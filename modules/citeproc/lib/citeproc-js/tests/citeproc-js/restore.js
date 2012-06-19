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

dojo.provide("citeproc_js.restore");

doh.registerGroup("citeproc_js.restore",
	[
		function testInstantiation() {
			var t = citeproc_js.restore;
			function testme () {
				if ("undefined" == typeof Item){
					Item = {"id": "Item-1"};
				}
				try {
					var sys = new RhinoTest();
					var style = new CSL.Engine(sys,t.csl);
					return "Success";
				} catch (e) {
					return "Failure";
				}
			}
			var res = testme();
			doh.assertEqual( "Success", res );
		},
		function testThatItWorksAtAll() {
			var t = citeproc_js.restore;
			function testme () {
				var sys, style, res1, res2, data;
				sys = new RhinoTest([t.item1, t.item2]);
				style = new CSL.Engine(sys,t.csl);
				try {
					[data, res1] = style.processCitationCluster(t.citation1, [], []);
					[data, res2] = style.processCitationCluster(t.citation2, [], []);
					style.restoreProcessorState([t.citation1x, t.citation2x]);
					return "Success";
				} catch (e) {
					return e;
				}
			}
			doh.assertEqual("Success", testme());
		},
		function testThatItWorksWithUndefinedSortkeysObject() {
			var t = citeproc_js.restore;
			function testme () {
				var sys, style, res1, res2, data;
				sys = new RhinoTest([t.item1, t.item2]);
				style = new CSL.Engine(sys,t.csl);
				try {
					delete t.citation1x.citationItems[0].sortkeys;
					delete t.citation1x.citationItems[1].sortkeys;
					delete t.citation2x.citationItems[0].sortkeys;
					style.restoreProcessorState([t.citation1x, t.citation2x]);
					return "Success";
				} catch (e) {
					return e;
				}
			}
			doh.assertEqual("Success", testme());
		},
		function testThatItWorksWithDuplicateCitationIDs() {
			var t = citeproc_js.restore;
			function testme () {
				var sys, style, res1, res2, data;
				sys = new RhinoTest([t.item1, t.item2]);
				style = new CSL.Engine(sys,t.csl);
				try {
					var res = style.restoreProcessorState([t.citation1x, t.citation2x, t.citation1xx]);
					return "Success";
				} catch (e) {
					return e;
				}
			}
			doh.assertEqual("Success", testme());
		},
		function testRestore() {
			var sys, style, res1, res2, res3, data;
			var t = citeproc_js.restore;
			sys = new RhinoTest([t.item1, t.item2, t.item3]);
			style = new CSL.Engine(sys,t.csl);
			[data, res1] = style.processCitationCluster(t.citation1, [], []);
			[data, res2] = style.processCitationCluster(t.citation2, [["CITATION-1", 1]], []);
			style.restoreProcessorState([t.citation1x, t.citation2x]);
			[data, res3] = style.processCitationCluster(t.citation3, [["CITATION-1", 1],["CITATION-2", 2]], []);
			doh.assertEqual("(Roe; J. Doe)", res3[0][1]);
			doh.assertEqual(2, res3.length);
			doh.assertEqual(0, res3[0][0]);
			doh.assertEqual("(Roe; J. Doe)", res3[0][1]);
		},
		function testEmptyRestore() {
			var sys, style, res1, res2, res3, data;
			var t = citeproc_js.restore;
			sys = new RhinoTest([t.item1, t.item2, t.item3]);
			style = new CSL.Engine(sys,t.csl);
			[data, res1] = style.processCitationCluster(t.citation1, [], []);
			[data, res2] = style.processCitationCluster(t.citation2, [["CITATION-1", 1]], []);
			style.restoreProcessorState();
			[data, res3] = style.processCitationCluster(t.citation3, [], []);
			doh.assertEqual(1, res3.length);
			doh.assertEqual(0, res3[0][0]);
			doh.assertEqual("(Doe)", res3[0][1]);
		}
	],
	function(){  //setup
		citeproc_js.restore.csl = "<style>"
			+ "<citation disambiguate-add-givenname=\"true\">"
			+ "  <sort>"
			+ "    <key variable=\"title\"/>"
			+ "  </sort>"
			+ "  <layout delimiter=\"; \" prefix=\"(\" suffix=\")\">"
			+ "    <names variable=\"author\">"
			+ "    <name form=\"short\" initialize-with=\". \"/>"
			+ "    </names>"
			+ "    <date variable=\"issued\" form=\"text\" date-parts=\"year\" prefix=\" \"/>"
			+ "  </layout>"
			+ "</citation>"
			+ "</style>";
		citeproc_js.restore.item1 = {
			"id": "ITEM-1",
			"type": "book",
			"title": "Book B",
			"author": [
				{
					"family": "Doe",
					"given": "John"
				}
			]
		};
		citeproc_js.restore.item2 = {
			"id": "ITEM-2",
			"type": "book",
			"title": "Book A",
			"author": [
				{
					"family": "Roe",
					"given": "Jane"
				}
			]
		};
		citeproc_js.restore.item3 = {
			"id": "ITEM-3",
			"type": "book",
			"title": "Book C",
			"author": [
				{
					"family": "Doe",
					"given": "Richard"
				}
			]
		};
		citeproc_js.restore.citation1 = {
			"citationID": "CITATION-1",
			"citationItems": [
				{
					"id": "ITEM-1"
				},
				{
					"id": "ITEM-2"
				}
			],
			"properties": {
				"index": 0,
				"noteIndex": 1
			}
		};
		citeproc_js.restore.citation1x = {
			"citationID": "CITATION-1",
			"citationItems": [
				{
					"id": "ITEM-1",
					"position": 0,
					"sortkeys": ["Book B"]
				},
				{
					"id": "ITEM-2",
					"position": 0,
					"sortkeys": ["Book A"]
				}
			],
			"properties": {
				"index": 0,
				"noteIndex": 1
			}
		};
		citeproc_js.restore.citation1xx = {
			"citationID": "CITATION-1",
			"citationItems": [
				{
					"id": "ITEM-1",
					"position": 0,
					"sortkeys": ["Book B"]
				},
				{
					"id": "ITEM-2",
					"position": 0,
					"sortkeys": ["Book A"]
				}
			],
			"properties": {
				"index": 0,
				"noteIndex": 1
			}
		};
		citeproc_js.restore.citation2 = {
			"citationID": "CITATION-2",
			"citationItems": [
				{
					"id": "ITEM-2"
				}
			],
			"properties": {
				"index": 1,
				"noteIndex": 2
			}
		};
		citeproc_js.restore.citation2x = {
			"citationID": "CITATION-2",
			"citationItems": [
				{
					"id": "ITEM-2",
					"position": 0,
					"sortkeys": ["Book A"]
				}
			],
			"properties": {
				"index": 1,
				"noteIndex": 2
			}
		};
		citeproc_js.restore.citation3 = {
			"citationID": "CITATION-3",
			"citationItems": [
				{
					"id": "ITEM-3"
				}
			],
			"properties": {
				"index": 2,
				"noteIndex": 3
			}
		};
	},
	function(){ // teardown
		delete citeproc_js.restore.csl;
		delete citeproc_js.restore.item1;
		delete citeproc_js.restore.item2;
		delete citeproc_js.restore.item3;
		delete citeproc_js.restore.citation1;
		delete citeproc_js.restore.citation1x;
		delete citeproc_js.restore.citation2;
		delete citeproc_js.restore.citation2x;
		delete citeproc_js.restore.citation3;
	}
);


var x = [
]