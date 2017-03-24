/**
* @file   testing/arbitrary_precision_calculation_tests.cpp
* @author Alexander Raß (alexander.rass@fau.de)
* @date   March, 2015
* @brief  This file contains methods for checking correct functionality of the arbitrary precision calculations.
*
* @copyright
* This project is released under the MIT License (MIT).
*
* @copyright
* The MIT License (MIT)
*
* @copyright
* Copyright (c) 2016 by Friedrich-Alexander-Universität Erlangen-Nürnberg and
* Alexander Raß
*
* @copyright
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* @copyright
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* @copyright
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#include <algorithm>
#include <assert.h>
#include <cfloat>
#include <cmath>
#include <gmp.h>
#include <iostream>
#include <string>
#include <vector>

#include "arbitrary_precision_calculation/arbitraryprecisioncalculation.h"

namespace arbitraryprecisioncalculation {

const double EPS = 1e-6;
const double PI = acos(-1.0);
const int RANDOM_NUMS = 1000;
const int BASE_PRECISION = 256;

unsigned int largePrecision;
unsigned int smallPrecision;

void init(){
	mpf_set_default_prec(BASE_PRECISION);
	smallPrecision = mpf_get_default_prec();
	mpf_set_default_prec(smallPrecision * 2);
	largePrecision = mpf_get_default_prec();
}


double maximalError = 0.0;

mpf_t* getError(mpf_t* pv1, mpf_t* pv2){
	mpf_t* v1 = arbitraryprecisioncalculation::mpftoperations::Clone(pv1);
	mpf_t* v2 = arbitraryprecisioncalculation::mpftoperations::Clone(pv2);
	mpf_t* a1 = arbitraryprecisioncalculation::mpftoperations::Abs(v1);
	mpf_t* a2 = arbitraryprecisioncalculation::mpftoperations::Abs(v2);
	mpf_t* ma = arbitraryprecisioncalculation::mpftoperations::Max(a1, a2);
	if(arbitraryprecisioncalculation::mpftoperations::Compare(ma, 1.0) > 0){
		mpf_t* tmp = arbitraryprecisioncalculation::mpftoperations::Divide(v1, ma);
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v1);
		v1 =  tmp;
		tmp = arbitraryprecisioncalculation::mpftoperations::Divide(v2, ma);
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v2);
		v2 = tmp;
	}
	mpf_t* dif = arbitraryprecisioncalculation::mpftoperations::Subtract(v1, v2);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v1);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v2);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(a1);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(a2);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(ma);
	mpf_t* err = arbitraryprecisioncalculation::mpftoperations::Abs(dif);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(dif);
	return err;
}

bool smallError(mpf_t* pv1, double d2){
	if(!(std::abs(d2) < DBL_MAX))return true;
	mpf_t* v2 = arbitraryprecisioncalculation::mpftoperations::ToMpft(d2);
	mpf_t* merr = getError(pv1, v2);
	double err = arbitraryprecisioncalculation::mpftoperations::MpftToDouble(merr);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v2);
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(merr);
	maximalError = std::max(maximalError, err);
	if(err >= EPS){
		std::cout << "error: " << err << std::endl;
	}
	return err < EPS;
}

double getMaximalError(){
	double res = maximalError;
	maximalError = 0.0;
	return res;
}

int testValues(std::vector<mpf_t*> checkNums,
		std::vector<double> expectedResults,
		std::vector<mpf_t*> actualResults,
		std::vector<mpf_t*> moreAccurateResults,
		int mpf_t_used){
	if(arbitraryprecisioncalculation::Configuration::isIncreasePrecisionRecommended()){
		arbitraryprecisioncalculation::Configuration::ResetIncreasePrecisionRecommended();
		std::cout << "WARNING: evaluation leads to an increase of precision\n";
	}
	{
		int result_should_be_true = (expectedResults.size() == checkNums.size());
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (expectedResults.size() == actualResults.size());
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (expectedResults.size() == moreAccurateResults.size());
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < expectedResults.size(); i++){
		if(!smallError(actualResults[i], expectedResults[i])){
			std::cerr << "WARNING: Error is not small but " << getMaximalError() << " for number ";
			std::cerr << arbitraryprecisioncalculation::mpftoperations::MpftToString(checkNums[i]);
			std::cerr << std::endl;
			std::cerr << "  actual result: " << arbitraryprecisioncalculation::mpftoperations::MpftToDouble(actualResults[i]) << std::endl;
			std::cerr << "expected result:" << expectedResults[i] << std::endl;
			assert(false);
			return 1;
		}
	}
	std::cout << "double precision tests passed - ";
	std::cout << "maximal error = " << getMaximalError() << std::endl;
	mpf_t* maxErr = arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0);

	for(unsigned int i = 0; i < checkNums.size(); i++){
		mpf_t* res1 = actualResults[i];
		mpf_t* res2 = moreAccurateResults[i];
		if(arbitraryprecisioncalculation::mpftoperations::Compare(res1, res2) == 0)continue;
		mpf_t* err = getError(res1, res2);
		if(arbitraryprecisioncalculation::mpftoperations::Compare(maxErr, err) < 0){
			std::swap(maxErr, err);
		}
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(err);
	}
	unsigned int minAccuracy = 0;
	while(arbitraryprecisioncalculation::mpftoperations::Compare(maxErr, 1.0) < 0 && minAccuracy < largePrecision){
		minAccuracy++;
		mpf_t* tmp = arbitraryprecisioncalculation::mpftoperations::Add(maxErr, maxErr);
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(maxErr);
		maxErr = tmp;
	}
	arbitraryprecisioncalculation::mpftoperations::ReleaseValue(maxErr);
	std::cout << "accuracy: min(rel error, abs error):" << minAccuracy << "/" << smallPrecision
		<< " = " << (100.0 * minAccuracy / (double) smallPrecision) << "%\n";
	if(minAccuracy < 0.5 * smallPrecision){
		std::cout << "WARNING: evaluation has bad precision!\n";
	}
	{
		int result_should_be_true = (minAccuracy > 0.2 * smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	arbitraryprecisioncalculation::vectoroperations::ReleaseValues(checkNums);
	arbitraryprecisioncalculation::vectoroperations::ReleaseValues(actualResults);
	arbitraryprecisioncalculation::vectoroperations::ReleaseValues(moreAccurateResults);
	if(arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() != mpf_t_used + arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached()){
		std::cerr << std::string(65, '!') << std::endl;
		std::cerr << "mpf_t's in use inconsistent. Should be " << mpf_t_used + arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached() << " but there are " << arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() << std::endl;
		std::cerr << std::string(65, '!') << std::endl;
		return 1;
	}
	arbitraryprecisioncalculation::Configuration::ResetIncreasePrecisionRecommended();
	return 0;
}

int testPi(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start Pi test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(PI);
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	{
		mpf_set_default_prec(smallPrecision);
		{
			int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
			assert(result_should_be_true);
			if(!(result_should_be_true)) return 1;
		}
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::GetPi());
		mpf_set_default_prec(largePrecision);
		{
			int result_should_be_true = (mpf_get_default_prec() == largePrecision);
			assert(result_should_be_true);
			if(!(result_should_be_true)) return 1;
		}
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::GetPi());
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testSin(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start sin test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(PI));
	expectedResults.push_back(0.0);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1e-50));
	expectedResults.push_back(sin(1e-50));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(PI * 0.5));
	expectedResults.push_back(sin(PI * 0.5));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(0.0);
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		if((dec & 6) == 6) v = 1/v;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(sin(v));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Sin(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Sin(checkNums[i]));
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testCos(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start cos test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(PI));
	expectedResults.push_back(cos(PI));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1e-50));
	expectedResults.push_back(cos(1e-50));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(PI * 0.5));
	expectedResults.push_back(cos(PI * 0.5));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(1.0);
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		if((dec & 6) == 6) v = 1/v;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(cos(v));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Cos(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Cos(checkNums[i]));
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testTan(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start tan test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(PI));
	expectedResults.push_back(tan(PI));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1e-50));
	expectedResults.push_back(tan(1e-50));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(PI * 0.5-1e-11));
	expectedResults.push_back(tan(PI * 0.5-1e-11));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(0.0);
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		if((dec & 6) == 6) v = 1/v;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(tan(v));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Tan(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Tan(checkNums[i]));
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testArcsin(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start arcsin test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(-1.0));
	expectedResults.push_back(-PI * 0.5);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::Negate(checkNums[0]));
	expectedResults.push_back( PI * 0.5);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1e-50));
	expectedResults.push_back(asin(1e-50));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(0.0);
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(asin(v));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Arcsin(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Arcsin(checkNums[i]));
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testArccos(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start arccos test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(-1.0));
	expectedResults.push_back(PI);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::Negate(checkNums[0]));
	expectedResults.push_back(0.0);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(PI * 0.5);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1.0-1e-14));
	expectedResults.push_back(acos(1.0-1e-14));
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(acos(v));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Arccos(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Arccos(checkNums[i]));
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testArctan(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start arctan test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::GetPlusInfinity());
	expectedResults.push_back(PI * 0.5);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::Negate(checkNums[0]));
	expectedResults.push_back(-PI * 0.5);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(0.0);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1e-50));
	expectedResults.push_back(asin(1e-50));
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(atan(v));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Arctan(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Arctan(checkNums[i]));
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testTrigonometric(){
	{
		int result_should_be_true = (testPi() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testSin() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testCos() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testTan() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testArctan() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testArcsin() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testArccos() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	return 0;
}

int testPowInt(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start powInt test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	std::vector<int> powers;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	powers.push_back(1);
	expectedResults.push_back(std::pow(0.0, 1));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	powers.push_back(0);
	expectedResults.push_back(std::pow(0.0, 0));
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		if(dec & 2) v *= 100;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		int p = rand() % 10;
		if(dec & 4) p = -p;
		powers.push_back(p);
		expectedResults.push_back(std::pow(v, p));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Pow(checkNums[i], powers[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Pow(checkNums[i], powers[i]));
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testPow(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start pow test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	std::vector<mpf_t*> powers;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	powers.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1.0));
	expectedResults.push_back(std::pow(0.0, 1));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	powers.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(std::pow(0.0, 0));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1.0));
	powers.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1.2132154564));
	expectedResults.push_back(1.0);
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 1) v = -v;
		if(dec & 2) v *= 100;
		double p;
		if(dec & 4){
			p = (rand() % 20);
			if(dec & 8){
				p *= 0.25;
				v = std::abs(v);
			}
			if(dec & 16) p = -p;
		} else {
			v = std::abs(v);
			p = rand() / (double) RAND_MAX;
			if(dec & 8) p = -p;
			if(dec & 16) p *= 100;
		}
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		powers.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(p));
		expectedResults.push_back(std::pow(v, p));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Pow(checkNums[i], powers[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Pow(checkNums[i], powers[i]));
	}
	arbitraryprecisioncalculation::vectoroperations::ReleaseValues(powers);
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testE(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start E test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(exp(1.0));
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	{
		mpf_set_default_prec(smallPrecision);
		{
			int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
			assert(result_should_be_true);
			if(!(result_should_be_true)) return 1;
		}
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::GetE());
		mpf_set_default_prec(largePrecision);
		{
			int result_should_be_true = (mpf_get_default_prec() == largePrecision);
			assert(result_should_be_true);
			if(!(result_should_be_true)) return 1;
		}
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::GetE());
	}
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testLog(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start log test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	std::vector<double> expectedResults2;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(1.0));
	expectedResults.push_back(0.0);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::GetPlusInfinity());
	expectedResults.push_back(INFINITY);
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 2) v *= 100;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(log(v));
	}
	for(unsigned int i = 0; i < checkNums.size(); i++) {
		expectedResults2.push_back(arbitraryprecisioncalculation::mpftoperations::Log2Double(checkNums[i]) * log(2));
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::LogE(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::LogE(checkNums[i]));
	}

	double merr = 0.0;
	for(unsigned int i = 0; i < checkNums.size(); i++){
		double r1 = expectedResults[i];
		double r2 = expectedResults2[i];
		if(std::abs(r1) > 1) {
			merr = std::max(merr, std::abs(r1-r2) / std::abs(r1));
		} else {
			merr = std::max(merr, std::abs(r1-r2));
		}
	}
	std::cout << "diff double vs double: " << merr << std::endl;
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testExp(){
	int mpf_t_used = arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached();
	std::cout << "start exp test\n";
	mpf_set_default_prec(BASE_PRECISION);
	std::vector<mpf_t*> checkNums;
	std::vector<double> expectedResults;
	std::vector<double> expectedResults2;
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::GetPlusInfinity());
	expectedResults.push_back(INFINITY);
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0));
	expectedResults.push_back(exp(0));
	checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::GetMinusInfinity());
	expectedResults.push_back(0);
	for(int i = 0; i <  RANDOM_NUMS; i++){
		double v = rand() / (double) RAND_MAX;
		int dec = rand();
		if(dec & 4) v = 1.0 / v;
		if(dec & 2) v *= 100;
		if(dec & 1) v *= -1;
		checkNums.push_back(arbitraryprecisioncalculation::mpftoperations::ToMpft(v));
		expectedResults.push_back(exp(v));
	}
	for(unsigned int i = 0; i < checkNums.size(); i++) {
		mpf_t* tmp = arbitraryprecisioncalculation::mpftoperations::Exp(checkNums[i]);
		expectedResults2.push_back(arbitraryprecisioncalculation::mpftoperations::MpftToDouble(tmp));
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(tmp);
	}
	std::vector<mpf_t*> actualResults;
	std::vector<mpf_t*> moreAccurateResults;
	mpf_set_default_prec(smallPrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == smallPrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		actualResults.push_back(arbitraryprecisioncalculation::mpftoperations::Exp(checkNums[i]));
	}
	mpf_set_default_prec(largePrecision);
	{
		int result_should_be_true = (mpf_get_default_prec() == largePrecision);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	for(unsigned int i = 0; i < checkNums.size(); i++){
		moreAccurateResults.push_back(arbitraryprecisioncalculation::mpftoperations::Exp(checkNums[i]));
	}

	double merr = 0.0;
	for(unsigned int i = 0; i < checkNums.size(); i++){
		double r1 = expectedResults[i];
		double r2 = expectedResults2[i];
		if(std::abs(r1) > 1) {
			merr = std::max(merr, std::abs(r1-r2) / std::abs(r1));
		} else {
			merr = std::max(merr, std::abs(r1-r2));
		}
	}
	std::cout << "diff double vs double: " << merr << std::endl;
	testValues(checkNums, expectedResults, actualResults, moreAccurateResults, mpf_t_used); 
	return 0;
}

int testLongLongToMpft(){
	std::cout << "start long_long_to_mpf_t test.\n";
	std::vector<long long> check_numbers = {0, -1, 1, 42, -7};
	for(int i = 4; i < 63; i++){
		for(int dif = -1; dif < 2; dif++){
			check_numbers.push_back((1LL << i) + dif);
		}
	}
	for(int i = 0; i < 100; i++){
		long long a = 0;
		long long m = 1LL << 62;
		while(m > 0){
			a *= RAND_MAX;
			a += rand();
			m /= RAND_MAX;
		}
		if(rand() % 2 == 0)a *= -1;
		check_numbers.push_back(a);
	}
	int prev_prec = mpf_get_default_prec();
	mpf_set_default_prec(64);
	for(auto v: check_numbers){
		auto a = v;
		mpf_t* direct = arbitraryprecisioncalculation::mpftoperations::ToMpft(v);
		bool negate = (v < 0);
		a = std::abs(a);
		mpf_t* cur = arbitraryprecisioncalculation::mpftoperations::ToMpft(0.0);
		mpf_t* mul = arbitraryprecisioncalculation::mpftoperations::ToMpft(1.0);
		while(a){
			if(a % 2 != 0){
				mpf_t* tmp = arbitraryprecisioncalculation::mpftoperations::Add(cur, mul);
				std::swap(tmp, cur);
				arbitraryprecisioncalculation::mpftoperations::ReleaseValue(tmp);
			}
			a /= 2;
			mpf_t* tmp = arbitraryprecisioncalculation::mpftoperations::Multiply(mul, 2.0);
			std::swap(tmp, mul);
			arbitraryprecisioncalculation::mpftoperations::ReleaseValue(tmp);
		}
		if(negate){
			mpf_t* tmp = arbitraryprecisioncalculation::mpftoperations::Negate(cur);
			std::swap(tmp, cur);
			arbitraryprecisioncalculation::mpftoperations::ReleaseValue(tmp);
		}
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(mul);
		{
			int result_should_be_true = (arbitraryprecisioncalculation::mpftoperations::Compare(cur, direct) == 0);
			assert(result_should_be_true);
			if(!(result_should_be_true)) return 1;
		}
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(cur);
		arbitraryprecisioncalculation::mpftoperations::ReleaseValue(direct);
	}
	mpf_set_default_prec(prev_prec);
	std::cout << "finished long_long_to_mpf_t test successfully.\n";
	return 0;
}

int signum(int v){
	if(v == 0)return 0;
	if(v < 0)return -1;
	if(v > 0)return 1;
	assert(false);
	return 1;
}

int testCompare(){
	std::vector<double> doubles = {INFINITY, -INFINITY, 0, 1e-300, -1e-300, 1.1e-300, 1.0, 1.0001, 1.000000001, 0.99999999999};
	for(int i = 0; i < 20; i++)doubles.push_back(rand());
	for(int i = (int)doubles.size() - 1; i >= 0; i--)doubles.push_back(-doubles[i]);
	for(unsigned int i = 0; i < doubles.size(); i++){
		for(unsigned int j = 0; j < doubles.size(); j++) {
			mpf_t* a = arbitraryprecisioncalculation::mpftoperations::ToMpft(doubles[i]);
			mpf_t* b = arbitraryprecisioncalculation::mpftoperations::ToMpft(doubles[j]);
			int cmp_res = 0;
			if(doubles[i] < doubles[j])cmp_res = -1;
			else if(doubles[i] > doubles[j])cmp_res = 1;
			int cmp_res1 = arbitraryprecisioncalculation::mpftoperations::Compare(a,b);
			int cmp_res2 = arbitraryprecisioncalculation::mpftoperations::Compare(a, doubles[j]);
			int cmp_res3 = arbitraryprecisioncalculation::mpftoperations::Compare(doubles[i], b);
			if(signum(cmp_res) != signum(cmp_res1)){
				std::cerr << "cmp1 failed " << doubles[i] << " " << doubles[j] << "(expected=" << cmp_res << " received=" << cmp_res1 << ")\n";
				assert(false);
				return 1;
			}
			if(signum(cmp_res) != signum(cmp_res2)){
				std::cerr << "cmp2 failed " << doubles[i] << " " << doubles[j] << "(expected=" << cmp_res << " received=" << cmp_res2 << ")\n";
				assert(false);
				return 1;
			}
			if(signum(cmp_res) != signum(cmp_res3)){
				std::cerr << "cmp3 failed " << doubles[i] << " " << doubles[j] << "(expected=" << cmp_res << " received=" << cmp_res3 << ")\n";
				assert(false);
				return 1;
			}
			mpf_t* max_ab = arbitraryprecisioncalculation::mpftoperations::Max(a,b);
			mpf_t* min_ab = arbitraryprecisioncalculation::mpftoperations::Min(a,b);
			double d_max_ab = std::max(doubles[i], doubles[j]);
			double d_min_ab = std::min(doubles[i], doubles[j]);
			double vg_min_ab = arbitraryprecisioncalculation::mpftoperations::MpftToDouble(min_ab);
			double vg_max_ab = arbitraryprecisioncalculation::mpftoperations::MpftToDouble(max_ab);
			{
				int result_should_be_true = (d_max_ab == vg_max_ab);
				if(!result_should_be_true){
					std::cerr << "max(" << doubles[i] << "," << doubles[j] << ")" << std::endl;
					std::cerr << d_max_ab << " == " << vg_max_ab << "?" << std::endl;
				}
				assert(result_should_be_true);
				if(!(result_should_be_true)) return 1;
			}
			{
				int result_should_be_true = (d_min_ab == vg_min_ab);
				if(!result_should_be_true){
					std::cerr << "min(" << doubles[i] << "," << doubles[j] << ")" << std::endl;
					std::cerr << d_min_ab << " == " << vg_min_ab << "?" << std::endl;
				}
				assert(result_should_be_true);
				if(!(result_should_be_true)) return 1;
			}
			arbitraryprecisioncalculation::mpftoperations::ReleaseValue(max_ab);
			arbitraryprecisioncalculation::mpftoperations::ReleaseValue(min_ab);
			arbitraryprecisioncalculation::mpftoperations::ReleaseValue(a);
			arbitraryprecisioncalculation::mpftoperations::ReleaseValue(b);
		}
	}
	return 0;
}

int testOperations(){
	{
		int result_should_be_true = (testPowInt() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testPow() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testE() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testLog() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testExp() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testLongLongToMpft() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testCompare() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	return 0;
}


int test_generateGaussianNoise(){
	std::cout << "test generateGaussianNoise\n";
	std::vector<double> mus  = {0.0, 0.0, 1.0, 1.1234, 5342.1};
	std::vector<double> vars = {1.0, 0.1, 1.0, 0.2213, 121.01};
	int test_size = 100000;
	double allowed_error = 0.01;
	double sq2 = sqrt( 2.0 );
	{
		int result_should_be_true = (mus.size() == vars.size());
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	unsigned int T = mus.size();
	mpf_set_default_prec(10);
	for(unsigned int t = 0; t < T; t++){
		double mu = mus[t], var = vars[t];
		double sigma = sqrt(var);
		std::cout << "check mu=" << mu << " var=" << var << std::endl;
		std::vector<double> values (test_size);
		for( int i = 0 ; i < test_size ; i++ ) {
			mpf_t* tmp = arbitraryprecisioncalculation::mpftoperations::GetGaussianRandomMpft(mu, sigma);
			values[i] = arbitraryprecisioncalculation::mpftoperations::MpftToDouble(tmp);
		}
		sort(values.begin(), values.end());
		for( int i = 0; i < test_size ; i++ ) {
			double expected_probability = ( erf((values[i] - mu) / (sigma * sq2)) + 1 ) * 0.5;
			double actual_probability1 = i / (double) (test_size);
			double actual_probability2 = ( i + 1.0 ) / (double) (test_size);
			double error1 = std::abs(expected_probability - actual_probability1);
			double error2 = std::abs(expected_probability - actual_probability2);
			if(std::max(error1, error2) > allowed_error) {
				std::cerr << "check failed!!\n";
				std::cerr << "at " << i << "-th smallest item with value " << values[i] << ":\n";
				std::cerr << "expected probability = " << expected_probability << std::endl;
				std::cerr << "actual probabilities = " << actual_probability1 << " and " << actual_probability2 << std::endl;
				std::cerr << "with error " << std::max(error1, error2 ) << ".\n";
				return 1;
			}
		}
		std::cout << "check succeeded.\n";
	}
	return 0;
}

int test_randomNumberGenerator(){
	std::cout << "begin random number generator test\n";
	std::vector<std::vector<std::string> > rng_descriptions ={
		{"0"},
		{"linearCongruenceRNG", "0", "standard", "fast"},
		{"linearCongruenceRNG", "0", "mod2p63" , "1571204578482947281", "12345678901234567", "fast"},
		{"linearCongruenceRNG", "0", "specific", "1571204578482947281", "12345678901234567", "9223372036854775808", "fast"}};

	std::vector<arbitraryprecisioncalculation::RandomNumberGenerator*> randoms;
	for(unsigned int i = 0; i < rng_descriptions.size(); i++){
		unsigned int parsed = 0;
		randoms.push_back(arbitraryprecisioncalculation::parse::ParseRandomNumberGenerator(rng_descriptions[i], parsed));
		assert(parsed == rng_descriptions[i].size());
		if(parsed != rng_descriptions[i].size())return 1;
		assert(randoms[i] != NULL);
		if(randoms[i] == NULL)return 1;
	}
	for(int i = 0; i < 10; i++){
		std::vector<long long> randll;
		std::vector<mpf_t*> randmpft;
		for(unsigned int j = 0; j < randoms.size(); j++){
			randll.push_back(randoms[j]->RandomLongLong());
			randmpft.push_back(randoms[j]->RandomMpft());
			assert(randll[j] == randll[0]);
			assert(mpf_cmp(*(randmpft[j]), *(randmpft[0])) == 0);
			if(randll[j] != randll[0]) return 1;
			if(mpf_cmp(*(randmpft[j]), *(randmpft[0])) != 0) return 1;
		}
		arbitraryprecisioncalculation::vectoroperations::ReleaseValues(randmpft);
	}
	arbitraryprecisioncalculation::RandomNumberGenerator* random = NULL;
	{
		unsigned int parsed = 0;
		random = arbitraryprecisioncalculation::parse::ParseRandomNumberGenerator(rng_descriptions[0], parsed);
	}
	long long value;
	int found_zeros = 0;
	for(unsigned long long iterations = 1; iterations <= (1ULL << 31); iterations++){
		value = random->RandomLongLong();
		if(value == 0){
			assert(iterations == (1ULL << 63));
			if(iterations != (1ULL << 63)){
				return 1;
			}
		}
		int current_zeros = 0;
		while(value % 2 == 0){
			++current_zeros;
			value /= 2;
		}
		if(current_zeros > found_zeros) {
			std::cout << "bits:"<< current_zeros << " periodicity:" << iterations << std::endl;
			assert(found_zeros == current_zeros - 1);
			if(found_zeros != current_zeros - 1){
				return 1;
			}
			found_zeros = current_zeros;
			assert(iterations == (1ULL << found_zeros));
			if(iterations != (1ULL << found_zeros)){
				return 1;
			}
		} else if( iterations == (1ULL << (found_zeros + 1)) ) {
			assert(false);
			return 1;
		}
	}
	std::cout << "random number generator test succeeded.\n";
	return 0;
}

int start_tests(int argv, char * argc[]) {
	arbitraryprecisioncalculation::Configuration::setOutputPrecision(10);
	arbitraryprecisioncalculation::Configuration::setInitialPrecision(BASE_PRECISION);
	arbitraryprecisioncalculation::Configuration::setPrecisionSafetyMargin(BASE_PRECISION / 2);
	{
		int result_should_be_true = (arbitraryprecisioncalculation::Configuration::isIncreasePrecisionRecommended() == false);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	mpf_set_default_prec(BASE_PRECISION);
	init();
	std::cout << "start test series\n";
	std::cout << "initial used mpf_t: " << arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached() << std::endl;
	std::cout << "initial memoized mpf_t: " << arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached() << std::endl;
	{
		int result_should_be_true = (testTrigonometric() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (testOperations() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	std::cout << "end test series\n";
	std::cout << "final used mpf_t: " << arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesInUse() - arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached() << std::endl;
	std::cout << "final memoized mpf_t: " << arbitraryprecisioncalculation::mpftoperations::GetNumberOfMpftValuesCached() << std::endl;
	{
		int result_should_be_true = (test_generateGaussianNoise() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	{
		int result_should_be_true = (test_randomNumberGenerator() == 0);
		assert(result_should_be_true);
		if(!(result_should_be_true)) return 1;
	}
	std::cout << "All tests passed.\n";
	return 0;
}

} // namespace arbitraryprecisioncalculation

int main(int argv, char * argc[]) {
	return arbitraryprecisioncalculation::start_tests(argv,argc);
}

