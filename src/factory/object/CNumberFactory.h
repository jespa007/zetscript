/*
 * CNumberObjectFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */
#pragma once





#define NEW_NUMBER				CNumberObjectFactory::newObject
#define GET_NUMBER				CNumberObjectFactory::getObjectResource

class CNumberObjectFactory: public CFactory<CNumber> {
public:


};
