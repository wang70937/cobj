/*
Copyright (c) 2014 xphh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "cobj.h"
#include <stdio.h>

/* 
 *  struct defined for test
 */
struct YearRecord 
{
	int year;
	CSTR yearLabel;
	DECLARE_LIST(struct MonthRecord 
	{
		int month;
		CSTR monthLabel;
		DECLARE_LIST(struct DayRecord 
		{
			int day;
			CSTR dayLabel;
			BOOL mark;
		}) dayList;
	}) monthList;
};

/* 
 *  you need to define metainfo for 'YearRecord' with 'YearRecord' together
 */
DEFINE_METAINFO(YearRecord)
{
	METAINFO_CREATE(YearRecord);

	METAINFO_ADD_MEMBER(YearRecord, FIELD_TYPE_INT, year);
	METAINFO_ADD_MEMBER(YearRecord, FIELD_TYPE_CSTR, yearLabel);

	METAINFO_CHILD_LIST_BEGIN(YearRecord, MonthRecord, monthList);

		METAINFO_ADD_MEMBER(MonthRecord, FIELD_TYPE_INT, month);
		METAINFO_ADD_MEMBER(MonthRecord, FIELD_TYPE_CSTR, monthLabel);
		
		METAINFO_CHILD_LIST_BEGIN(MonthRecord, DayRecord, dayList);

			METAINFO_ADD_MEMBER(DayRecord, FIELD_TYPE_INT, day);
			METAINFO_ADD_MEMBER(DayRecord, FIELD_TYPE_CSTR, dayLabel);
			METAINFO_ADD_MEMBER(DayRecord, FIELD_TYPE_BOOL, mark);

		METAINFO_CHILD_END();

	METAINFO_CHILD_END();
}

int main()
{
	/* 
	*  remember to register metainfo of 'YearRecord' in your implement code
	*/
	REGISTER_METAINFO(YearRecord);

	{
		CSTR json;
		struct YearRecord *rec = OBJECT_NEW(YearRecord);

		rec->year = 2014;
		rec->yearLabel = CS("Hangzhou, China"); // CSTR must be initialized with 'CS' macro

		{
			// you can add an empty object into list first
			LIST_ADD_EMPTY(rec->monthList, MonthRecord);
			rec->monthList.array[0].month = 11;
			rec->monthList.array[0].monthLabel = CS("Nov.");

			LIST_ADD_EMPTY(rec->monthList.array[0].dayList, DayRecord);
			LIST_ADD_EMPTY(rec->monthList.array[0].dayList, DayRecord);
			LIST_ADD_EMPTY(rec->monthList.array[0].dayList, DayRecord);
			rec->monthList.array[0].dayList.array[2].day = 4;
			rec->monthList.array[0].dayList.array[2].dayLabel = CS("Today nothing to do.");
			rec->monthList.array[0].dayList.array[2].mark = 1;
		}

		{
			// also object that is on stack can be add into list
			// make sure that the object must zero memory
			struct MonthRecord month_rec = {0};
			month_rec.month = 12;
			month_rec.monthLabel = CS("next month");
			LIST_ADD_OBJ(rec->monthList, month_rec);
		}

		// convert to json string
		json = OBJECT_TO_JSON(rec, YearRecord);
		printf(json.cstr);

		// clear object but not free
		OBJECT_CLEAR(rec, YearRecord);

		// delete 'rec' (free allocated memory), instead of 'free' function
 		OBJECT_DELETE(rec, YearRecord);

		{
			// convert from json string
			struct YearRecord *rec2 = OBJECT_FROM_JSON(YearRecord, json.cstr);

 			OBJECT_DELETE(rec2, YearRecord);
		}

		// the output json string is CSTR type, do not forget to free it!
		CS_CLEAR(json);
	}

	getchar();
	return 0;
}
