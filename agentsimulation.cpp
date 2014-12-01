

#include <iostream>
#include <vector>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <functional>   // std::minus
#include <numeric>      // std::accumulate
#include <math.h>  
#include <cmath> 
#include <algorithm>
#include <fstream>


using namespace std;

long long currenttimestep_global = 0;


double mymean(vector<double>& v){
	
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	double mean = sum / v.size();
	return mean;
}



double mystd(vector<double>& v){
	
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	double mean = sum / v.size();
	double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
	double stdev = std::sqrt(sq_sum / v.size() - mean * mean);
	return stdev;
}




struct order{
	long long time1;
	long currentstep;
	double price;
	double quant;
	bool isbuy;
	order(){
		currentstep = currenttimestep_global;
	}
};





double get_traderpnl(vector<order> os,double theoprice){

	double avgbuyprice = 0;
	double avgsellprice = 0;
	double numbuyshares = 0;
	double numsellshares = 0;
	for(int i = 0;i<os.size();++i){
		if(os.at(i).isbuy){
			avgbuyprice += os.at(i).quant*os.at(i).price;
			numbuyshares += os.at(i).quant;
		}
		else{
			avgsellprice += os.at(i).quant*os.at(i).price;
			numsellshares += os.at(i).quant;
		}
	}

	avgbuyprice/=max(numbuyshares,1.0);
	avgsellprice/=max(numsellshares,1.0);
	cout<<"numshares"<<" "<<numbuyshares<<" "<<numsellshares<<" "<<theoprice<<" "<<avgbuyprice<<" "<<avgsellprice<<" "<<
	(theoprice-avgbuyprice)*numbuyshares + (avgsellprice-theoprice)*numsellshares <<std::endl;
	return (theoprice-avgbuyprice)*numbuyshares + (avgsellprice-theoprice)*numsellshares ;
	
}




struct orderlevel{
	vector<order> bids;
	vector<order> offers;
	double price;
	double bidsum;
	double asksum;
	orderlevel(){
		bidsum = 0;
		asksum = 0;
	}

};

struct orderbook{
	vector<orderlevel> levels;
	int topbid_loc;
	int topask_loc;

void printbook(){
	double bidsum = 0;
	double asksum = 0;
	cout<<"topbidlocation "<<topbid_loc<<" topaskloc "<<topask_loc<<std::endl;
	for(int i = topbid_loc-4;i<=topask_loc+4;++i){
		bidsum = 0;
		asksum = 0;
		for(int j = 0;j<levels.at(i).bids.size();++j)
			bidsum += levels.at(i).bids.at(j).quant;

		for(int j = 0;j<levels.at(i).offers.size();++j)
			asksum += levels.at(i).offers.at(j).quant;

		cout<<i<<" "<<levels.at(i).price<<" "<<bidsum<<" "<<asksum<<" "<<levels.at(i).bidsum<<" "<<
		levels.at(i).asksum<<" "<<levels.at(i).bids.size()<<" "<<levels.at(i).offers.size()<<endl;
	
	}	



}
 double get_midprice(){
 	return (levels.at(topbid_loc).price +  levels.at(topask_loc).price )/2.0;
 }


double get_best_bid_price(){
	return levels.at(topbid_loc).price;
}



double get_best_ask_price(){
	return levels.at(topask_loc).price;
}




double get_ratio3(){
 	
 	double bidsum = 0.0;
 	double asksum = 0.0;
 	for(int i = max(topbid_loc-2,0);i<=topbid_loc;++i){
 		bidsum += levels.at(i).bidsum;
 	}

	for(int i = min(topask_loc+2,999);i>=topask_loc;--i){
 		asksum += levels.at(i).asksum;
 	}

 	//return bidsum + asksum;
 	return (bidsum-asksum)/max(bidsum+asksum,.1);
 }


double get_booksz(double& bidsum,double& asksum){
 	
 	bidsum = 0.0;
 	asksum = 0.0;
 	for(int i = max(topbid_loc-2,0);i<=topbid_loc;++i){
 		bidsum += levels.at(i).bidsum;
 	}

	for(int i = min(topask_loc+2,999);i>=topask_loc;--i){
 		asksum += levels.at(i).asksum;
 	}

 	//return bidsum + asksum;
 	//return (bidsum-asksum)/max(bidsum+asksum,.1);
 }





double get_ratio1(){


}

void cancelorders(){

	for(int i =topbid_loc;i>=max(topbid_loc-2,0);--i){
			int j = 0;
			while(j<levels.at(i).bids.size()){
				//cout<<"leveltime "<<levels.at(i).bids.at(j).time1<<std::endl;
				if(currenttimestep_global - levels.at(i).bids.at(j).currentstep >100){
					//cout<<"did cancelbid "<<levels.at(i).bids.at(j).quant<<std::endl;
					levels.at(i).bidsum -= levels.at(i).bids.at(j).quant;
					levels.at(i).bids.erase(levels.at(i).bids.begin()+j);
					if(levels.at(i).bids.size()==0 && topbid_loc==i){
						--topbid_loc;					
					}
				}
				else
					++j;
			}
	}
 	

	for(int i = topask_loc; i<=min(topask_loc+2,999);++i){
 		int j = 0;
			while(j<levels.at(i).offers.size()){
				if(currenttimestep_global - levels.at(i).offers.at(j).currentstep >100){
					//cout<<"did canceloffer "<<levels.at(i).offers.at(j).currentstep<<std::endl;
					levels.at(i).asksum -= levels.at(i).offers.at(j).quant;
					levels.at(i).offers.erase(levels.at(i).offers.begin()+j);

					if(levels.at(i).offers.size()==0 && topask_loc==i){
						++topask_loc;					
					}

				}
				else
					++j;
			}
 	}



}

 void addorder(orderbook& ob,order& o1,bool isbuy,int level_offset,vector<order>& trades){
 	try{
 		trades.clear();
 		const double tol = .0001;
 		const int max_level = 999;
 		int top_order;
 		int top_opposite;
 		int levelloc;
 		order temporder;
 		int orderloc;
 		if(level_offset>=0){ //limit order
 			if(isbuy){
 				orderloc = min(max(ob.topbid_loc-level_offset,0),max_level); 
 				//cout<<"bid order loc "<<ob.topbid_loc<<" "<<level_offset<<" "<<orderlocendl;
 				ob.levels.at(orderloc).bids.push_back(o1);
 				ob.levels.at(orderloc).bidsum += o1.quant;
 			}
 			else{
				orderloc = min(max(ob.topask_loc+level_offset,0),max_level); 
 				ob.levels.at(orderloc).offers.push_back(o1);
 				ob.levels.at(orderloc).asksum  += o1.quant;
 			}
 		}
 		else{//price improvement or trade against opposite
 			int spread = ob.topask_loc - ob.topbid_loc;
 			
 			if(spread==2){//price improvement
 				//temporder.size = o1.size;
 				if(isbuy){
 					ob.topbid_loc++;
 					//ob.topbid = min(max_level,ob.topbid_loc);
 					ob.levels.at(ob.topbid_loc).bids.push_back(o1);
 					ob.levels.at(ob.topbid_loc).bidsum = o1.quant;
 				}
 				else{
					ob.topask_loc--;
					//ob.topask_loc = max(0,ob.topask_loc);
 					ob.levels.at(ob.topask_loc).offers.push_back(o1);
 					ob.levels.at(ob.topask_loc).asksum = o1.quant;
 				}
 			}
 			else{//trade against opposite side
 				int i  =0;
 				double osz = o1.quant;
 				double sz_remaining = osz;
 				double trdamt;
 				std::vector<order>* bookorders;
 				double* sumorders;
 				double price;
 				if(isbuy){
 					bookorders =  &ob.levels.at(ob.topask_loc).offers;
 					price = ob.levels.at(ob.topask_loc).price;
 					sumorders = &ob.levels.at(ob.topask_loc).asksum;
 					//cout<<"sumorders before"<<" "<<*sumorders<<" "<<o1.quant<<std::endl;
 				}
 				else{
 					bookorders =  &ob.levels.at(ob.topbid_loc).bids;
 					price =   ob.levels.at(ob.topbid_loc).price;
 					sumorders = &ob.levels.at(ob.topbid_loc).bidsum;
 				//	cout<<"sumorders before"<<" "<<*sumorders<<" "<<o1.quant<<std::endl;
 				}

 				double quant;
 				//while(sz_remaining>tol){
 				trdamt == 0;
 				while(bookorders->size() && sz_remaining>tol){
 					//cout<<"determining trade bksz="<<bookorders->size()<<"sz_remaining "<<sz_remaining<<" "<<trdamt<<std::endl;
 					quant = bookorders->at(i).quant;
 					//cout<<"size of book "<<quant<<endl;
 					if(sz_remaining - quant>-tol){
 						sz_remaining -= quant;
 						trdamt += quant;
 						bookorders->erase(bookorders->begin()+i);
 						*sumorders -= quant;
 						//cout<<"sumorders after 1"<<" "<<*sumorders<<" "<<quant<<std::endl;
 					}
 					else{
 						bookorders->at(i).quant -= sz_remaining;
 						trdamt += sz_remaining;
 						*sumorders -= sz_remaining;
 						sz_remaining = 0.0;
 						
 				//		cout<<"sumorders after 2"<<" "<<*sumorders<<" "<<quant<<std::endl;
 					}
 				}	
 				//}
 				//if(level_offset==-1)
 				//	cout<<"tradeamt "<<trdamt<<std::endl;
 				if(trdamt>tol){
 					order o1;
 					o1.isbuy = isbuy;
 					o1.quant = trdamt;
 					o1.price = price; 
 					trades.push_back(o1);	
 				}
 				//cout<<"sz remaining "<<sz_remaining<<" "<<bookorders->size()<<endl;
 				if(sz_remaining<=tol && bookorders->size() ==0 ){
 				//	cout<<"took entire level"<<std::endl;
 				//	double aa;
 				//	cin>>aa;
 					if(isbuy)
 						ob.topask_loc++;
 					else
 						ob.topbid_loc--;


 				}
 				//we bought all the existing level and moved level1 increment
 				if(sz_remaining>tol){
 					order temporder;
 					temporder.quant = sz_remaining;
 					
 					if(isbuy){
 						ob.topbid_loc++;
 						ob.topask_loc++;
 						ob.levels.at(ob.topbid_loc).bids.push_back(temporder);
 						ob.levels.at(ob.topbid_loc).bidsum = temporder.quant;
 						if(ob.levels.at(ob.topbid_loc).asksum>.01 ){
 							cout<<"this shouldn't happen"<<std::endl;
 							cout<<"at new bidlevel level but there is some quant on the ask"<<std::endl;
 						}
 					}
 					else{
 						ob.topask_loc--;
 						ob.topbid_loc--;
 						ob.levels.at(ob.topask_loc).offers.push_back(temporder);
 						ob.levels.at(ob.topask_loc).asksum = temporder.quant;
 						if(ob.levels.at(ob.topask_loc).bidsum>.01 ){
 							cout<<"this shouldn't happen"<<std::endl;
 							cout<<"at new ask level but there is some  quant on the bid"<<std::endl;
 						}
 					}

 				}
 					
 			}
 			


 		}




	}
catch(...){


 }
}

void subtract_order(order&o1, bool isbuy){
	try{
		

	}
	catch(...){

	}



}


int find_loc(double price,bool isbid){





}



};





void initialize_orderbook(orderbook& o){

//initialize orderbook

	orderlevel o1;
	double price = 0.0;
	for(int i = 0;i<1000;++i){
		price += .1;
		o1.price = price;
		o.levels.push_back(o1);
		cout<<i<<" "<<price<<endl;
	
	}
	o.topbid_loc = 500;
	o.topask_loc = 501;

	order order1;
	order1.quant = 50;
	order1.time1 = 0;
	for(int i =  0;i<=500;++i){
		o.levels.at(i).bids.push_back(order1);
		o.levels.at(i).bidsum = order1.quant;
		o.levels.at(i).asksum = 0;
	}

	

	for(int i =  501;i<=999;++i){
		o.levels.at(i).offers.push_back(order1);
		o.levels.at(i).asksum = order1.quant;
		o.levels.at(i).bidsum = 0;
	}


}

struct datapoint{
	int timestep;
	double bidsum;
	double asksum;

};



void myplaceorder1( orderbook& ob1,bool isbuy,double quant,vector<order>&  temptrades){
	try{

		order o1;
		o1.quant = quant;
		o1.isbuy = isbuy;
		ob1.addorder(ob1,o1,isbuy,-1,temptrades);

	}
	catch(...){
		cout<<"placeorder1 error";
	}



}


int main()
{

	const int numsteps = 1000;
	const int numsimulations = 1000;
	std::vector<datapoint> bookdata;
	orderbook ob;
	orderbook default_ob  ;
	initialize_orderbook(default_ob);
	//default_ob.printbook();
	//int nnn;
	//cin>>nnn;
	double startprice = default_ob.get_midprice();
	vector<double> closingprices(numsimulations);
	vector<double> ratio3vec(numsteps), rtnvec(numsteps);
	order o1;
	o1.quant = 1000;
	vector<order> trades;
	ob.addorder(ob,o1,true,2,trades);
	ob.addorder(ob,o1,true,3,trades);
	vector<order> allorders;
	vector<double> trader1_pnls(numsimulations);
	srand(time(NULL));


	
	double whichlevel;
	bool isbuy;

	ofstream out1("outrtns.txt");
	double endprice;
	vector<order> trader1_trades,temptrades;
	double trader1_position;
	vector<double> enter_bidsum,enter_asksum,leave_bidsum,leave_asksum,bidsums,asksums;
	int mytradecount ;
	datapoint tempdatapoint;
	// double gg  get_traderpnl(trader1_trades,endprice)get_traderpnl(trader1_trades,endprice);
	for(int h = 0;h<numsimulations;++h){
		if(h%100==0)
			cout<<" SIMNUMBER "<<h<<endl;
			ob = default_ob;
		//	ob.printbook();	
		//	cin>>isbuy;
		trader1_trades.clear();
		trader1_position = 0;
		mytradecount = 0;
		currenttimestep_global = 0;	
		


		for(int i = 0;i<numsteps;++i){

			double bidsum1,asksum1;
			ob.get_booksz(tempdatapoint.bidsum,tempdatapoint.asksum);
			tempdatapoint.timestep = i;
			bookdata.push_back(tempdatapoint);
		//cout<<tempdatapoint.timestep<<" "<<tempdatapoint.bidsum<<" "<<tempdatapoint.asksum<<std::endl;
		
			if(tempdatapoint.bidsum<.1){
				ob.printbook();
				int xx = 0;
				cin>>xx;
			}



	
		
		bidsums.push_back(bidsum1);
		asksums.push_back(asksum1);

			//cout<<"stepnum "<<h<<endl;
			++currenttimestep_global;
			if(false && i>5 && mytradecount<20  && ob.topask_loc - ob.topbid_loc==1){
				order o1;
				mytradecount++;
				
				double bidsum1,asksum1;
				ob.get_booksz(bidsum1,asksum1);

				enter_bidsum.push_back(bidsum1);
				enter_asksum.push_back(asksum1);
				leave_bidsum,leave_asksum;

				double quant;
				double isbuy;
				//o1.quant = ob.levels.at(ob.topask_loc).asksum;
				quant = ob.levels.at(ob.topask_loc).asksum;
				isbuy = true;
				//o1.isbuy = true;
				trader1_position  += quant;
				myplaceorder1(ob,isbuy,quant,temptrades);


				//sell case
				/*
				quant = ob.levels.at(ob.topbid_loc).asksum;
				isbuy = false;
				trader1_position  -= quant;
				myplaceorder1(ob,isbuy,quant,temptrades);
				*/


				//ob.printbook();

				//ob.addorder(ob,o1,true,-1,temptrades);
				//cout<<"XXXXXXXX\n";
				//ob.printbook();
				
				/*				
				o1.quant = ob.levels.at(ob.topbid_loc).bidsum;
				o1.isbuy = false;
				trader1_position  -= o1.quant;
				//ob.printbook();
				ob.addorder(ob,o1,false,-1,temptrades);
				*/
				//cout<<mytradecount<<" XXXXXXX\n";
				//ob.printbook();
				//int xxx;
				//cin>>xxx;
				//cout<<"tradesz="<<temptrades.size()<<std::endl;
				//if(temptrades.size())
					//cout<<"tradeinfo "<<temptrades.at(0).price<<" "<<temptrades.at(0).quant<<std::endl;
				//cout<<std::endl;
				//cout<<"after sell "<<ob.get_midprice()<<std::endl;
				//ob.printbook();
				if(temptrades.size())
					trader1_trades.insert(trader1_trades.end(),temptrades.begin(),temptrades.end());
			}

			if(i==10){
				//double nn;
				//cin>>nn;

			}

			if(trader1_position>100 && ob.topask_loc - ob.topbid_loc==1 && i>900){
				order o1;
				//cout<<"closed trade"<<endl;
				double bidsum1,asksum1;
				ob.get_booksz(bidsum1,asksum1);

				leave_bidsum.push_back(bidsum1);
				leave_asksum.push_back(asksum1);
				


				
				o1.quant = min(ob.levels.at(ob.topbid_loc).bidsum,trader1_position);
				o1.isbuy = false;
				trader1_position  -= o1.quant;
				
				/*
				o1.quant = min(ob.levels.at(ob.topask_loc).asksum,trader1_position);
				o1.isbuy = true;
				trader1_position  += o1.quant;
				*/
 				//void addorder(orderbook& ob,order& o1,bool isbuy,int level_offset,vector<order>& trades)
			
				ob.addorder(ob,o1,o1.isbuy,-1,temptrades);
				if(temptrades.size())
					trader1_trades.insert(trader1_trades.end(),temptrades.begin(),temptrades.end());
			}

			isbuy = rand()/(double(RAND_MAX)+1) >.5;
			if(i >10){
				//ob.printbook();
				//cout<<"xxx "<<o1.isbuy<<" "<<o1.quant<<std::endl;
				//int xxx;
				//cin>>xxx;
			}
			whichlevel = int(rand()/(double(RAND_MAX)+1)*7.0)-1;
			if(whichlevel>=2)
				whichlevel =-1;
			//cout<<whichlevel<<" "<<isbuy<<endl;
			//ob.cancelorders();
			ob.addorder(ob,o1,isbuy,whichlevel,trades);
			if(trades.size())
				allorders.insert(allorders.end(),trades.begin(),trades.end());
			
			 ratio3vec.at(i) = ob.get_ratio3();
			 rtnvec.at(i) = ob.get_midprice();	
		//	cout<<"step i "<<i<<"midprice "<<ob.get_midprice()<<" "<<ob.get_ratio3()<<" "<<whichlevel<<" "<<isbuy<<" "<<ob.get_best_bid_price()<<" "<<ob.get_best_ask_price()<<" "<<
		//	ob.topbid_loc<<" "<<ob.topask_loc<<" "<<ob.levels.at(ob.topbid_loc).bids.size()<<" "<<
		//	ob.levels.at(ob.topask_loc).offers.size()<<" "<<ob.levels.at(ob.topbid_loc).bidsum<<" "<<
		//	ob.levels.at(ob.topask_loc).asksum<<endl;


		}

		endprice = ob.get_midprice();
		closingprices.at(h) = endprice;
		trader1_pnls.at(h) = get_traderpnl(trader1_trades,endprice);
		for(int i = 0;i<numsteps;++i){
			rtnvec.at(i) = endprice - rtnvec.at(i) ;		
			//out1<<rtnvec.at(i)<<" "<<ratio3vec.at(i)<<std::endl;
		}
	}

	default_ob.printbook();
	ob.printbook();	

	//cout<<"numtrades "<<allorders.size()<<std::endl;
	//cout<<"isbuy\t\tquant\t\tprice"<<std::endl;
	std::ofstream out1a("bookinfo.txt");
	for(int i = 0;i<bookdata.size();++i)
		out1a<<tempdatapoint.timestep<<" "<<tempdatapoint.bidsum<<" "<<
		tempdatapoint.asksum<<std::endl;

		


	cout<<"average closing price "<<mymean(closingprices)<<" "<<mystd(closingprices)<<" "<<*min_element(closingprices.begin(), closingprices.end())<<
	" "<<*max_element(closingprices.begin(), closingprices.end())<<" trade pnl "<<mymean(trader1_pnls)<<" trade std "<<mystd(trader1_pnls)<<
	" "<<accumulate(enter_bidsum.begin(),enter_bidsum.end(),0)/double(enter_bidsum.size())<<" "<<
	" "<<accumulate(enter_asksum.begin(),enter_asksum.end(),0)/double(enter_asksum.size())<<" "<<
" "<<accumulate(leave_bidsum.begin(),leave_bidsum.end(),0)/double(leave_bidsum.size())<<" "<<
	" "<<accumulate(leave_asksum.begin(),leave_asksum.end(),0)/double(leave_asksum.size())<<" "<<
	" "<<accumulate(bidsums.begin(),bidsums.end(),0)/double(bidsums.size())<<" "<<
	" "<<accumulate(asksums.begin(),asksums.end(),0)/double(asksums.size())<<" "<<
	std::endl;
	//for(int i = 0;i<allorders.size();++i)
	//	cout<<allorders.at(i).isbuy<<"\t\t"<<allorders.at(i).quant<<"\t\t"<<
	//allorders.at(i).price<<endl;
	cout << "End simulation!" << endl;
	return 0;
}
