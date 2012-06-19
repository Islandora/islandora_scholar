/*
 * Copyright (c) 2009, 2010 and 2011 Frank G. Bennett, Jr. All Rights
 * Reserved.
 *
 * The contents of this file are subject to the Common Public
 * Attribution License Version 1.0 (the “License”); you may not use
 * this file except in compliance with the License. You may obtain a
 * copy of the License at:
 *
 * http://bitbucket.org/fbennett/citeproc-js/src/tip/LICENSE.
 *
 * The License is based on the Mozilla Public License Version 1.1 but
 * Sections 14 and 15 have been added to cover use of software over a
 * computer network and provide for limited attribution for the
 * Original Developer. In addition, Exhibit A has been modified to be
 * consistent with Exhibit B.
 *
 * Software distributed under the License is distributed on an “AS IS”
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is the citation formatting software known as
 * "citeproc-js" (an implementation of the Citation Style Language
 * [CSL]), including the original test fixtures and software located
 * under the ./std subdirectory of the distribution archive.
 *
 * The Original Developer is not the Initial Developer and is
 * __________. If left blank, the Original Developer is the Initial
 * Developer.
 *
 * The Initial Developer of the Original Code is Frank G. Bennett,
 * Jr. All portions of the code written by Frank G. Bennett, Jr. are
 * Copyright (c) 2009, 2010 and 2011 Frank G. Bennett, Jr. All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU Affero General Public License (the [AGPLv3]
 * License), in which case the provisions of [AGPLv3] License are
 * applicable instead of those above. If you wish to allow use of your
 * version of this file only under the terms of the [AGPLv3] License
 * and not to allow others to use your version of this file under the
 * CPAL, indicate your decision by deleting the provisions above and
 * replace them with the notice and other provisions required by the
 * [AGPLv3] License. If you do not delete the provisions above, a
 * recipient may use your version of this file under either the CPAL
 * or the [AGPLv3] License.”
 */

dojo.provide("citeproc_js.dateparse");

var sys = new RhinoTest();
var citeproc = new CSL.Engine(sys,"<style></style>");

var keycount = function(obj){
    var c=0;
    for (pos in obj) {
      c+=1;
    }
    return c;
};

doh.register("tests.dateparse", [    function test_dateparse029() {
        var res = citeproc.fun.dateparser.parse("Aug 15 2000 - Aug 20 2000");
        doh.assertEqual("2000", res["year_end"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual("20", res["day_end"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("8", res["month_end"]);
        doh.assertEqual("15", res["day"]);
        doh.assertEqual(6, keycount(res) );
		},
function test_dateparse001() {
        var res = citeproc.fun.dateparser.parse("Wed 24 Oct 2000");
        doh.assertEqual("10", res["month"]);
        doh.assertEqual("24", res["day"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual(3, keycount(res) );
    },
    function test_dateparse002() {
        var res = citeproc.fun.dateparser.parse("\u5e73\u621012\u5e7410\u670824\u65e5");
        doh.assertEqual("10", res["month"]);
        doh.assertEqual("24", res["day"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual(3, keycount(res) );
    },
    function test_dateparse003() {
        var res = citeproc.fun.dateparser.parse("19??-10");
        doh.assertEqual("10", res["month"]);
        doh.assertEqual("19??", res["year"]);
        doh.assertEqual(2, keycount(res) );
    },
    function test_dateparse004() {
        var res = citeproc.fun.dateparser.parse("myauntsally 23");
        doh.assertEqual("myauntsally 23", res["literal"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse005() {
        var res = citeproc.fun.dateparser.parse("\"[Dec 23, 2009]\"");
        doh.assertEqual("[Dec 23, 2009]", res["literal"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse006() {
        var res = citeproc.fun.dateparser.parse("Aug 31, 2000");
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(3, keycount(res) );
    },
    function test_dateparse007() {
        var res = citeproc.fun.dateparser.parse("31 Aug 2000");
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(3, keycount(res) );
    },
    function test_dateparse008() {
        var res = citeproc.fun.dateparser.parse("08-31-2000");
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(3, keycount(res) );
    },
    function test_dateparse009() {
        var res = citeproc.fun.dateparser.parse("2000-8-31");
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(3, keycount(res) );
    },
    function test_dateparse010() {
        var res = citeproc.fun.dateparser.parse("Sum 2000");
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("14", res["month"]);
        doh.assertEqual(2, keycount(res) );
    },
    function test_dateparse011() {
        var res = citeproc.fun.dateparser.parse("Trinity 2001");
        doh.assertEqual("Trinity", res["season"]);
        doh.assertEqual("2001", res["year"]);
        doh.assertEqual(2, keycount(res) );
    },
    function test_dateparse012() {
        var res = citeproc.fun.dateparser.parse("Spring 2000 - Summer 2001");
        doh.assertEqual("14", res["month_end"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("2001", res["year_end"]);
        doh.assertEqual("13", res["month"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse013() {
        var res = citeproc.fun.dateparser.parse("circa 08-31-2000");
        doh.assertEqual("1", res["circa"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse014() {
        var res = citeproc.fun.dateparser.parse("circa 2000-31-08");
        doh.assertEqual("1", res["circa"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse015() {
        var res = citeproc.fun.dateparser.parse("circa Aug 31, 2000");
        doh.assertEqual("1", res["circa"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse016() {
        var res = citeproc.fun.dateparser.parse("Aug 31 2000 ?");
        doh.assertEqual("1", res["circa"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse017() {
        var res = citeproc.fun.dateparser.parse("[31 Aug 2000?]");
        doh.assertEqual("1", res["circa"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("31", res["day"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse018() {
        var res = citeproc.fun.dateparser.parse("200BC");
        doh.assertEqual("-200", res["year"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse019() {
        var res = citeproc.fun.dateparser.parse("200bc");
        doh.assertEqual("-200", res["year"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse020() {
        var res = citeproc.fun.dateparser.parse("200 b.c.");
        doh.assertEqual("-200", res["year"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse021() {
        var res = citeproc.fun.dateparser.parse("250AD");
        doh.assertEqual("250", res["year"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse022() {
        var res = citeproc.fun.dateparser.parse("250ad");
        doh.assertEqual("250", res["year"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse023() {
        var res = citeproc.fun.dateparser.parse("250 a.d.");
        doh.assertEqual("250", res["year"]);
        doh.assertEqual(1, keycount(res) );
    },
    function test_dateparse024() {
        var res = citeproc.fun.dateparser.parse("2000-2001");
        doh.assertEqual("2001", res["year_end"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual(2, keycount(res) );
    },
    function test_dateparse025() {
        var res = citeproc.fun.dateparser.parse("Aug - Sep 2000");
        doh.assertEqual("9", res["month_end"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual("2000", res["year_end"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse026() {
        var res = citeproc.fun.dateparser.parse("Aug 15-20 2000");
        doh.assertEqual("2000", res["year_end"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual("20", res["day_end"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("8", res["month_end"]);
        doh.assertEqual("15", res["day"]);
        doh.assertEqual(6, keycount(res) );
    },
    function test_dateparse027() {
        var res = citeproc.fun.dateparser.parse("Aug 2000-Sep 2000");
        doh.assertEqual("9", res["month_end"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual("2000", res["year_end"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual(4, keycount(res) );
    },
    function test_dateparse028() {
        var res = citeproc.fun.dateparser.parse("\u5e73\u621012\u5e748\u6708\u301c\u5e73\u621012\u5e749\u6708");
        doh.assertEqual("9", res["month_end"]);
        doh.assertEqual("8", res["month"]);
        doh.assertEqual("2000", res["year"]);
        doh.assertEqual("2000", res["year_end"]);
        doh.assertEqual(4, keycount(res) );
    }
]);
