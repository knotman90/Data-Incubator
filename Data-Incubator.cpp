//============================================================================
// Name        : Data-Incubator.cpp
// Author      : Davide Spataro
// Version     :
// Copyright   : 
// Description : Question 1
//============================================================================

#include <stdio.h>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <iostream>
/*
 *
 * Q1: Let's play JackBlack.
 * We have a large deck of cards A, 2, 4, 8, 16, 32, 64
 * in equal proportion.
 * Each card is worth the number of points inscribed on it
 * with A being worth one point.
 * You draw cards iteratively until the sum of their
 * point values is greater than or equal to NN.
 * The difference of the sum and NN is your score.
 *
 *The difference of the sum and N is your score.
 *
 */

template< typename Iterator  , typename OutStream = std::ostream>
void print(const Iterator start, const Iterator end){
	for(auto i=start;i!=end;i++){
		std::cout << *i << "\t";
	}
	std::cout << std::endl;
}


#define KIND_A (1)



//assume that the cards are mixed evenly
template<class T>
class Game{
public:
	const T N;
	double mean;
	double stdev;
	std::vector<T> data;

	int cards[7];


	void reset(){
		mean =0.0d;
		stdev=0.0d;
		data.clear();
	}
	//methods
	Game(uint lim ) : N(lim), mean(0.0d),stdev(0.0d) {
		for(int i=0;i<7;i++){
			cards[i] = 1 << i;
		}
	} ;

	void setA(int value){
		cards[0] = value;
	}

	T getCard(){
		uint c = rand() % 7 ;
		return cards[c];
	}

	double computeVariance(std::vector<T>& data){
		std::vector<double> diff(data.size());
		std::transform(data.begin(), data.end(), diff.begin(),
				std::bind2nd(std::minus<double>(), mean));
		double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		int nT = data.size();
		return sq_sum / nT;
	}
	/*
	 * What is the mean of your score if N=SOMETHING
	 * What is the standard deviation of your score if N=SOMETHING
	 */
	void play(int nTrial){
		const int nT = nTrial;
		T totalScore =0;
		data.clear();
		for(; nTrial > 0 ; nTrial--){
			T trialScore =0;
			T point=0;
			T numC = 0;
			for(; point < N ; numC++){
				uint c = getCard();
				point += c;
			}
			trialScore = (point-N);
			data.push_back(trialScore);
			totalScore+=trialScore;
		}

		this->mean = (double)totalScore/(double)nT;

		std::vector<double> diff(data.size());
		std::transform(data.begin(), data.end(), diff.begin(),
				std::bind2nd(std::minus<double>(), mean));
		double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);

		double variance = sq_sum / nT;
		double standardDeviation = std::sqrt(variance);

		stdev = standardDeviation;
		printf("\tN=%i , mean = %.10f variance %.10f, stdev = %.10f\n",N,mean,variance,stdev);
	}


	void playConditional(int nTrial, int startScore , int upperBound){
		const int nT = nTrial;
		T score_LE_5 =0;
		T numTimes_startScore =0;

		for(; nTrial > 0 ; nTrial--){
			T trialScore = 0;
			T point= 0;
			T numC = 0;
			bool drawn_startScore = false;
			for(; point < N ; numC++){
				int c = getCard();
				//printf("%i ",c);
				if(!drawn_startScore && c == startScore)
					drawn_startScore=true;

				point += c;
			}
			if(drawn_startScore){//startScore was drawn
				numTimes_startScore++; //num times 8 is drawn
				trialScore = (point-N);
				if(trialScore <= upperBound)
					score_LE_5++; //num times score <= 5 AND 8 is drawn
			}

			//printf("point=%i, numc=%i, trialScore=%i, ok=%i, num_times8=%i, numLE5=%i\n", point, numC,trialScore,drawn_startScore,numTimes_startScore,score_LE_5);
		}

		printf("\nnum_times8=%i, numLE5=%i\n",numTimes_startScore,score_LE_5);
		double p_b = (double)numTimes_startScore/(double)nT;
		double p_a_AND_b = (double) score_LE_5/(double)nT;

		double P_a_IF_b = p_a_AND_b /p_b;
		printf("\tp_b = %.10f, p_aANDB=%.10f, RES = %.10f\n",p_b,p_a_AND_b,P_a_IF_b);

	}


	/*
	 * What is the mean of your score if N=SOMETHING
	 * What is the standard deviation of your score if N=SOMETHING
	 */
	void play_optimize(int nTrial){
		const int nT = nTrial;
		const int original_valA = cards[0];
		setA(1);
		T totalScore =0;
		for(; nTrial > 0 ; nTrial--){
			//drawn cards using A=1
			std::vector<int> trialCard;
			T point_one=0;
			T numC_one = 0;
			T num_aces=0;
			for(; point_one < N ; numC_one++){
				uint c = getCard();
				point_one+=c;
				trialCard.push_back(c);
				if(c==1)
					num_aces++;
			}
			//print(trialCard.begin(),trialCard.end());
			//printf("\tnum_extractions %i, num_aces = %i, point %i\n",numC_one,num_aces,point_one);
			int bestVal = computeBestScore(trialCard,0,0);

			T trialScore = (bestVal-N);
			data.push_back(trialScore);
			totalScore+=trialScore;
			//printf("BEST VALUE %i score = %i \n",bestVal,trialScore);
			//printf("%i,",trialScore);
		}//for trials

		mean = (double)totalScore/(double)nT;
		stdev= std::sqrt(computeVariance(data));

		printf("\tN=%i , mean = %.10f , stdev = %.10f\n",N,mean,stdev);

		setA(original_valA);
	}


	T computeBestScore(std::vector<int>& extracted,int used,int point){


		if(point >= N){
			return point;
		}


		uint c = extracted[used];
		//printf("extracted: %i\n ",c);
		if(c !=1 ){
			point+=c;
			return computeBestScore(extracted,used+1,point);
		}
		else{// ace extracted
			T val_ace1 = computeBestScore(extracted,used+1,point+1);
			T val_ace11 = computeBestScore(extracted,used+1,point+11);
			return std::min(val_ace1,val_ace11);
		}


	}

	T computeBestScoreAndSequence(std::vector<int>& extracted,int used,int point, std::vector<int>& best){


			if(point >= N){
				best.erase(best.begin()+used,best.end());
				return point;
			}


			uint c = extracted[used];
			//printf("extracted: %i\n ",c);
			if(c !=1 ){
				point+=c;
				best[used] =c;
				return computeBestScoreAndSequence(extracted,used+1,point,best);
			}
			else{// ace extracted
				T val_ace1 = computeBestScoreAndSequence(extracted,used+1,point+1,best);
				T val_ace11 = computeBestScoreAndSequence(extracted,used+1,point+11,best);
				if(val_ace1 < val_ace11){
					best[used] = 1;
				}else{
					best[used] = 11;
				}
				return std::min(val_ace1,val_ace11);
			}


		}



};

#define NTRIALS (1000000)

int main() {
	srand(time(0));
	Game<int> g21(21);
	Game<int> g1000( 1000 );


	//question 1.1
	printf("\n\n\t\tQUESTION 1.1 \n\n");

	printf("What is the mean/STD of your score if N=21\n");
	g21.play(NTRIALS);

	printf("\nWhat is the mean/STD of your score if N=1000\n");
	g1000.play(NTRIALS);

	g21.reset();
	g1000.reset();
	//question 1.2
	printf("\n\n\t\t QUESTION 1.2 \n\n");

	printf("\nWhat is the conditionalProb if N=21\n");
	g21.playConditional(NTRIALS,8,5);

	printf("\nWhat is the conditionalProb if N=1000\n");
	g1000.playConditional(NTRIALS,8,5);


	//optimize
	/*
	//STATIC i.e. ace =1 or ace=11 all extraction (11 better)
	g21.setA(1);
	g21.play(NTRIALS);
	double mean21_1 = g21.mean;

	g21.reset();
	g21.setA(11);
	g21.play(NTRIALS);
	double mean21_11 = g21.mean;

	printf("21 - MEAN 1 - %.10f\n",mean21_1);
	printf("21 - MEAN 11 - %.10f\n",mean21_11);

	//choose best for 1000
	g1000.setA(1);
	g1000.play(NTRIALS);
	double mean1000_1 = g1000.mean;

	g1000.reset();
	g1000.setA(11);
	g1000.play(NTRIALS);
	double mean1000_11 = g1000.mean;

	printf("1000 - MEAN 1 - %.10f\n",mean1000_1);
	printf("1000 - MEAN 11 - %.10f\n",mean1000_11);
	 */
	//dynamic optimize
	printf("\n\n\t\t QUESTION 1.3 \n\n");
	g21.reset();
	g1000.reset();

	printf("\nWhat is the mean/STD of your OPTIMIZED score if N=21\n");
	g21.play_optimize(10000);
	printf("\nWhat is the mean/STD of your OPTIMIZED score if N=1000\n");
	g1000.play_optimize(10000);
	return 0;


	return 0;
}
