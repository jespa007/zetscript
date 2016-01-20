/*
 * CStringObjectFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */
#pragma once

#define NEW_STRING				CStringObjectFactory::newObject
#define GET_STRING				CStringObjectFactory::getObjectResource

class CStringObjectFactory: public CFactory<CString> {
public:



};
