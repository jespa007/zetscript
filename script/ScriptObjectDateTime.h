/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectDateTime: public ScriptObject{
	public:
		static ScriptObjectDateTime * newScriptObjectDateTime(ZetScript *_zs);
		static bool isLeapYear(int year);
		static int maxDay(int month, int year);
		static bool isLeapDay(int day, int month, int year);



		ScriptObjectDateTime();

		bool isValid() const;

		bool isLeapYear() const;
		bool isLeapDay() const;

		int getYear() const {if(isValid() == false){} return _year;}
		int getMonth() const {if(isValid() == false){} return _month;}
		int getDay() const {if(isValid() == false){} return _day;}


		void addDay(bool forward);



		void addYears(int years);
		void addMonths(int months);
		void addDays(int days);

	protected:

		int _second;
		int _minute;
		int _hour;

		int _year;
		int _month;
		int _day;
		static const std::string MONTH_STRINGS[];
		static const int DAYS_PER_MONTH[];
		static const int MIN_MONTH = 1;
		static const int MAX_MONTH = 12;
		static const int MIN_DAY = 1;
	};

}
