/*
 * CVectorObjectFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */
#pragma once

#define NEW_VECTOR				CVectorFactory::newObject
#define GET_VECTOR				CVectorFactory::getObjectResource

class CVectorFactory: public CFactory<CVector> {
public:

	static void registerScriptFunctions();

};
