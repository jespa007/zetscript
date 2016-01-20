/*
 * CObjectFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */

#pragma once



#define NEW_OBJECT				CObjectFactory::newObject
#define GET_OBJECT				CObjectFactory::getObjectResource


class CObjectFactory: public CFactory<CObject> {



public:



};
