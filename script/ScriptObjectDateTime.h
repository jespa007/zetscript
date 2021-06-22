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



		void addSeconds(int _seconds);
		void addMinutes(int _minutes);
		void addHours(int _hours);
		void addDays(int days);
		void addMonths(int months);
		void addYears(int years);

		int getWeekDay() const;
		int getMonthDay() const;
		int getYearDay() const;

		int getSecond() const;
		int getMinute() const;
		int getHour() const;
		int getDay() const;
		int getMonth() const;
		int getYear() const;

		std::string toString(const std::string & _format);
		virtual std::string toString();

	protected:

		zs_datetime datetime;

		bool isLeapYear() const;
	};

}
