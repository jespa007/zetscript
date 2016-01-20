/*
 * CBooleanFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */
#pragma once





#define NEW_BOOLEAN				CBooleanFactory::newObject
#define GET_BOOLEAN				CBooleanFactory::getObjectResource

class CBooleanFactory: public CFactory<CBoolean> {
public:



};
