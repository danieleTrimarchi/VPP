/*
 * IOutils.h
 *
 *  Created on: Jun 21, 2016
 *      Author: dtrimarchi
 */

#include <stdio.h>
#include <iostream>
#include "VPPException.h"
#include "Warning.h"
#include "mathUtils.h"

using namespace std;

#ifndef IOUTILS_H_
#define IOUTILS_H_

class IOUtils {

	public:

		/// Constructor
		IOUtils(WindItem* pWind) :
			pWindItem_(pWind) {

		};

		/// Ask the user to prompt the values of twv and twa
		void askUserWindIndexes(size_t& twv, size_t& twa){

			if(!pWindItem_)
				throw VPPException(HERE, "pWind == 0!");

			std::cout<<"--> Please enter the values of twv and twa for the Induced Resistance plot: "<<std::endl;
			while(true){
			cin >> twv >> twa;
			bool vFine= twv < pWindItem_->getWVSize();
			bool aFine= twa < pWindItem_->getWASize();
			if(!vFine)
				std::cout<<"the value of twv is out of range, max is: "<<pWindItem_->getWVSize()-1<<std::endl;
			if(!aFine)
				std::cout<<"the value of twa is out of range, max is: "<<pWindItem_->getWASize()-1<<std::endl;
			if(vFine&&aFine)
				break;
			}

			std::cout<<" twv["<<twv<<"] = "<<pWindItem_->getTWV(twv)<<" [m/s]"<<std::endl;
			std::cout<<" twa["<<twa<<"] = "<<mathUtils::toDeg(pWindItem_->getTWA(twa))<<" [deg]"<<std::endl;
		};

		/// Ask the user to prompt the value of the state vector
		void askUserStateVector(Eigen::VectorXd& x){

			// Resize the state vector to the current size of the VPP problem
			x.resize(4);

			std::cout<<"--> Please enter the values the state vector: "<<std::endl;
			for(size_t i=0; i<x.size(); i++) cin >> x(i);

			std::cout<<"--> Got: "<<x<<std::endl;

		};

// Not sure if this works. In the context of inducedResistanceItem::plot calling
// this method somehow sent a break of the outer command loop in main, thus the
// termination of the program...?
//		/// Ask the user a boolean with a given message and return its value
//		bool askUserBool(string msg) {
//
//			bool ans;
//			std::cout<<msg<<std::endl;
//			cin >> ans;
//			return ans;
//
//		}


		/// Destructor
		~IOUtils(){};

	private:

		/// Ptr to the wind item
		WindItem* pWindItem_;
};


#endif /* IOUTILS_H_ */
