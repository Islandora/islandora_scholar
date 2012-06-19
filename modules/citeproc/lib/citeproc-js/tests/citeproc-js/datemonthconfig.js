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

dojo.provide("citeproc_js.datemonthconfig");

CSL.debug = print;

doh.register("tests.datemonthconfig", [
		function test_ExtendAmbigMonthAbbrev() {
			var parser = new CSL.DateParser;
			parser.addMonths("jan januberry mar apr may jun jul aug sep oct nov dec");
			doh.assertEqual(2, parser.mabbrevs[1].length);
			doh.assertEqual("feb", parser.mabbrevs[1][0]);
			doh.assertEqual("janub", parser.mabbrevs[1][1]);
		},
        function test_SimpleParse() {
			var parser = new CSL.DateParser;
			var res = parser.parse("Wed 24 Oct 2000");
			doh.assertEqual("10", res["month"]);
			doh.assertEqual("24", res["day"]);
			doh.assertEqual("2000", res["year"]);
		},
		function test_SimpleReset() {
			var parser = new CSL.DateParser;
			parser.resetMonths();
			var res = parser.parse("Wed 24 Oct 2000");
			doh.assertEqual("10", res["month"]);
			doh.assertEqual("24", res["day"]);
			doh.assertEqual("2000", res["year"]);
		},
		function test_ExtendBadLength() {
			var parser = new CSL.DateParser;
			parser.addMonths("januberry");
			var res = parser.parse("Wed 24 Oct 2000");
			doh.assertEqual("10", res["month"]);
			doh.assertEqual("24", res["day"]);
			doh.assertEqual("2000", res["year"]);
		},
		function test_ExtendSameMonthAbbrev() {
			var parser = new CSL.DateParser;
			parser.addMonths("jan februberry mar apr may jun jul aug sep oct nov dec");
			doh.assertEqual(1, parser.mabbrevs[1].length);
			doh.assertEqual("feb", parser.mabbrevs[1][0]);
		},
		function test_ExtendSimpleNewMonthAbbrev() {
			var parser = new CSL.DateParser;
			parser.addMonths("jan fezruary mar apr may jun jul aug sep oct nov dec");
			doh.assertEqual(2, parser.mabbrevs[1].length);
			doh.assertEqual("feb", parser.mabbrevs[1][0]);
			doh.assertEqual("fez", parser.mabbrevs[1][1]);
		},
		function test_ExtendAmbigMonthAbbrevTwice() {
			var parser = new CSL.DateParser;
			parser.addMonths("jan januberry mar apr may jun jul aug sep oct nov dec");
			doh.assertEqual(2, parser.mabbrevs[1].length);
			doh.assertEqual("feb", parser.mabbrevs[1][0]);
			doh.assertEqual("janub", parser.mabbrevs[1][1]);
			parser.addMonths("jan bebruary mar apr may jun jul aug sep oct nov dec");
			doh.assertEqual(3, parser.mabbrevs[1].length);
			doh.assertEqual("beb", parser.mabbrevs[1][2]);
			doh.assertEqual(1, parser.mabbrevs[2].length);
		}
]);
