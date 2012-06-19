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

dojo.provide("citeproc_js.emptybib");
doh.registerGroup("citeproc_js.emptybib",
	[
		function testInstantiation() {
			var t = citeproc_js.emptybib;
			var res = false;
			try {
				var citeproc = new CSL.Engine(t.sys, t.csl_nobib);
				res = "Success";
			} catch (e) {
				res = "Failure";
			}
			doh.assertEqual( "Success", res );
		},
		function testNoBibliographyAtAll() {
			var t = citeproc_js.emptybib;
			var citeproc = new CSL.Engine(t.sys, t.csl_nobib);
			citeproc.updateItems(["ITEM-1"]);
			var res = citeproc.makeBibliography();
			doh.assertEqual(true, res === false);
		},
		function testEmptyBibliographyEntry() {
			var t = citeproc_js.emptybib;
			var citeproc = new CSL.Engine(t.sys, t.csl_hasbib);
			citeproc.updateItems(["ITEM-1", "ITEM-2", "ITEM-3", "ITEM-4"]);
			var res = citeproc.makeBibliography();
			// One error returned, with value of [2, "ITEM-3", 1]
			doh.assertEqual(1, res[0].bibliography_errors.length);
			doh.assertEqual(2, res[0].bibliography_errors[0].index);
			doh.assertEqual("ITEM-3", res[0].bibliography_errors[0].itemID);
			doh.assertEqual(1, res[0].bibliography_errors[0].error_code);
		},
		function testEmptyBibliographyEntryExceptCitationNumber() {
			var t = citeproc_js.emptybib;
			var citeproc = new CSL.Engine(t.sys, t.csl_hasnumberedbib);
			citeproc.updateItems(["ITEM-1", "ITEM-2", "ITEM-3", "ITEM-4"]);
			var res = citeproc.makeBibliography();
			// One error returned, with value of [2, "ITEM-3", 1]
			doh.assertEqual(1, res[0].bibliography_errors.length);
			doh.assertEqual(2, res[0].bibliography_errors[0].index);
			doh.assertEqual("ITEM-3", res[0].bibliography_errors[0].itemID);
			doh.assertEqual(CSL.ERROR_NO_RENDERED_FORM, res[0].bibliography_errors[0].error_code);
		},
		function testBibliographyOk() {
			var t = citeproc_js.emptybib;
			var citeproc = new CSL.Engine(t.sys, t.csl_hasbib);
			citeproc.updateItems(["ITEM-1", "ITEM-2", "ITEM-4"]);
			var res = citeproc.makeBibliography();
			// One error returned, with value of [2, "ITEM-3", 1]
			doh.assertEqual(0, res[0].bibliography_errors.length);
		}
	],
	function () {
		var t = citeproc_js.emptybib;
		var Sys = function () {
			var ITEMS = {
				"ITEM-1": {
					"id": "ITEM-1",
					"title": "Title One"
				},
				"ITEM-2": {
					"id": "ITEM-2",
					"title": "Title Two"
				},
				"ITEM-3": {
					"id": "ITEM-3"
				},
				"ITEM-4": {
					"id": "ITEM-4",
					"title": "Title Four"
				},
			};
			this.retrieveItem = function (id) {
				return ITEMS[id];
			};
			this.retrieveLocale = function (lang) {
				return "<locale xmlns=\"http://purl.org/net/xbiblio/csl\" version=\"1.0\" xml:lang=\"en\">"
					   + "<terms>"
					   + "  <term name=\"open-quote\"></term>"
					   + "  <term name=\"close-quote\"></term>"
					   + "  <term name=\"open-inner-quote\"></term>"
					   + "  <term name=\"close-inner-quote\"></term>"
					   + "  <term name=\"ordinal-01\"></term>"
					   + "  <term name=\"ordinal-02\"></term>"
					   + "  <term name=\"ordinal-03\"></term>"
					   + "  <term name=\"ordinal-04\"></term>"
					   + "  <term name=\"long-ordinal-01\"></term>"
					   + "  <term name=\"long-ordinal-02\"></term>"
					   + "  <term name=\"long-ordinal-03\"></term>"
					   + "  <term name=\"long-ordinal-04\"></term>"
					   + "  <term name=\"long-ordinal-05\"></term>"
					   + "  <term name=\"long-ordinal-06\"></term>"
					   + "  <term name=\"long-ordinal-07\"></term>"
					   + "  <term name=\"long-ordinal-08\"></term>"
					   + "  <term name=\"long-ordinal-09\"></term>"
					   + "  <term name=\"long-ordinal-10\"></term>"
					   + "</terms>"
					   + "</locale>";
			};
		};
		t.sys = new Sys();
		t.csl_nobib = "<style>"
					+ "<citation>"
					+ "  <layout>"
					+ "    <text variable=\"title\"/>"
					+ "  </layout>"
					+ "</citation>"
			+ "</style>";
		t.csl_hasbib = "<style>"
					+ "<citation>"
					+ "  <layout>"
					+ "    <text variable=\"title\"/>"
					+ "  </layout>"
					+ "</citation>"
					+ "<bibliography>"
					+ "  <layout>"
					+ "    <text variable=\"title\"/>"
					+ "  </layout>"
					+ "</bibliography>"
			+ "</style>";
		t.csl_hasnumberedbib = "<style>"
					+ "<citation>"
					+ "  <layout>"
					+ "    <text variable=\"title\"/>"
					+ "  </layout>"
					+ "</citation>"
					+ "<bibliography>"
					+ "  <layout>"
					+ "    <text variable=\"citation-number\" prefix=\"[\" suffix=\"] \"/>"
					+ "    <text variable=\"title\"/>"
					+ "  </layout>"
					+ "</bibliography>"
			+ "</style>";
		t.makeTestBib = function (ids) {

		};
	},
	function () {}
);
