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

dojo.provide("citeproc_js.preview");

var mycsl = "<style>"
	  + "<citation disambiguate-add-givenname=\"true\">"
	  + "  <layout delimiter=\"; \" prefix=\"(\" suffix=\")\">"
	  + "    <names variable=\"author\">"
	  + "    <name form=\"short\" initialize-with=\". \"/>"
	  + "    </names>"
	  + "    <date variable=\"issued\" form=\"text\" date-parts=\"year\" prefix=\" \"/>"
	  + "  </layout>"
	  + "</citation>"
	+ "</style>";

var mycsl2 = "<style>"
	  + "<citation disambiguate-add-givenname=\"true\" disambiguate-add-year-suffix=\"true\">"
	  + "  <layout delimiter=\"; \" prefix=\"(\" suffix=\")\">"
	  + "    <names variable=\"author\">"
	  + "    <name form=\"short\" initialize-with=\". \"/>"
	  + "    </names>"
	  + "    <date variable=\"issued\" form=\"text\" date-parts=\"year\" prefix=\" \"/>"
	  + "  </layout>"
	  + "</citation>"
	+ "</style>";

var ITEM1 = {
	"id": "ITEM-1",
	"type": "book",
	"author": [
		{
			"family": "Doe",
			"given": "John"
		},
		{
			"family": "Roe",
			"given": "Jane"
		}
	]
};

var ITEM2 = {
	"id": "ITEM-2",
	"type": "book",
	"author": [
		{
			"family": "Doe",
			"given": "John"
		},
		{
			"family": "Roe",
			"given": "Richard"
		}
	]
};

var ITEM3 = {
	"id": "ITEM-3",
	"type": "book",
	"author": [
		{
			"family": "Wallbanger",
			"given": "Harvey"
		},
		{
			"family": "Smith",
			"given": "Horatio"
		}
	],
	"issued": {
		"date-parts": [
			[
				1999
			]
		]
	}
};


var ITEM4 = {
	"id": "ITEM-4",
	"type": "book",
	"author": [
		{
			"family": "Wallbanger",
			"given": "Harvey"
		},
		{
			"family": "Smith",
			"given": "Horatio"
		}
	],
	"issued": {
		"date-parts": [
			[
				1999
			]
		]
	}
};

var ITEM5 = {
	"id": "ITEM-5",
	"type": "book",
	"author": [
		{
			"family": "Gershwin",
			"given": "George"
		}
	],
	"issued": {
		"date-parts": [
			[
				1999
			]
		]
	}
};


var ITEM6 = {
	"id": "ITEM-6",
	"type": "book",
	"author": [
		{
			"family": "Gershwin",
			"given": "George"
		}
	],
	"issued": {
		"date-parts": [
			[
				1999
			]
		]
	}
};



var CITATION1 = {
	"citationID": "CITATION-1",
	"citationItems": [
		{
			"id": "ITEM-1"
		}
	],
	"properties": {
		"index": 0,
		"noteIndex": 1
	}
};

var CITATION2 = {
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

var CITATION2x = {
	"citationID": "CITATION-2",
	"citationItems": [
		{
			"id": "ITEM-2"
		}
	],
	"properties": {
		"index": 0,
		"noteIndex": 1
	}
};

var CITATION3 = {
	"citationID": "CITATION-3",
	"citationItems": [
		{
			"id": "ITEM-1"
		}
	],
	"properties": {
		"index": 1,
		"noteIndex": 2
	}
};

var CITATION4 = {
	"citationID": "CITATION-4",
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

var CITATION5 = {
	"citationID": "CITATION-5",
	"citationItems": [
		{
			"id": "ITEM-1"
		}
	],
	"properties": {
		"index": 1,
		"noteIndex": 2
	}
};



var CITATION6 = {
	"citationItems": [
		{
			"id": "ITEM-3"
		},
		{
			"id": "ITEM-4"
		}
	],
	"properties": {
		"index": 0,
		"noteIndex": 0
	}
};

var CITATION7 = {
	"citationItems": [
		{
			"id": "ITEM-5"
		},
		{
			"id": "ITEM-6"
		}
	],
	"properties": {
		"index": 0,
		"noteIndex": 0
	}
};



doh.register("citeproc_js.preview", [
	function testInstantiation() {
		function testme () {
			if ("undefined" == typeof Item){
				Item = {"id": "Item-1"};
			}
			try {
				var sys = new RhinoTest();
				var style = new CSL.Engine(sys,mycsl);
				return "Success";
			} catch (e) {
				return "Failure";
			}
		}
		var res = testme();
		doh.assertEqual( "Success", res );
	},
	function testThatItWorksAtAll() {
		var sys = new RhinoTest([ITEM1]);
		var style = new CSL.Engine(sys,mycsl);
		var res = style.previewCitationCluster(CITATION1, [], [], "html");
		doh.assertEqual("(Doe, Roe)", res);
	},
	function testOverlayPreview() {
		var sys, style, res1, res2, data;
		sys = new RhinoTest([ITEM1]);
		style = new CSL.Engine(sys,mycsl);
		[data, res1] = style.processCitationCluster(CITATION1, [], []);
		res2 = style.previewCitationCluster(CITATION1, [], [], "html");
		doh.assertEqual("(Doe, Roe)", res1[0][1]);
		doh.assertEqual("(Doe, Roe)", res2);
	},
	function testRollbackGivennameDisambig() {
		var sys, style, res1, res2, res3, data;
		sys = new RhinoTest([ITEM1, ITEM2]);
		style = new CSL.Engine(sys,mycsl);
		[data, res1] = style.processCitationCluster(CITATION1, [], []);
		res2 = style.previewCitationCluster(CITATION2, [["CITATION-1", 1]], [], "html");
		[data, res3] = style.processCitationCluster(CITATION3, [["CITATION-1", 1]], []);
		//doh.assertEqual("(Doe, Roe)", res1[0][1]);
		//doh.assertEqual("(Doe, R. Roe)", res2);
		//doh.assertEqual(1, res3.length);
		doh.assertEqual("(Doe, Roe)", res3[0][1]);
	},
	function testInitialsNeededOnlyWithOriginalCitationItemContent() {
		var sys, style, res1, res2, res3, res4, res5, data;
		sys = new RhinoTest([ITEM1, ITEM2, ITEM3]);
		style =new CSL.Engine(sys,mycsl);
		[data, res1] = style.processCitationCluster(CITATION1, [], []);
		[data, res2] = style.processCitationCluster(CITATION2, [["CITATION-1", 1]], []);
		// names rendered are:
		//   C1i1: John Doe, Jane Roe
		//   C2i2: John Doe, Richard Roe
		//   C4i3: Harvey Wallbanger, Horatio Smith
		res3 = style.processCitationCluster(CITATION4, [["CITATION-1", 1], ["CITATION-2", 2]], []);
		// names rendered are:
        //   C1i1: John Doe, Jane Roe
		//   C5i1: John Doe, Jane Roe
		//   C4i3: Harvey Wallbanger, Horatio Smith
		res4 = style.previewCitationCluster(CITATION5, [["CITATION-1", 1]], [["CITATION-4", 3]], "html", CITATION2);
		doh.assertEqual("(Doe, Roe)", res4);
		// same as before preview:
		//   C1i1: John Doe, Jane Roe
		//   C2i2: John Doe, Richard Roe
		//   C4i3: Harvey Wallbanger, Horatio Smith
		[data, res5] = style.processCitationCluster(CITATION2, [["CITATION-1", 1]], [["CITATION-4", 3]]);
		// This is the critical test in this fixture: if the processor
		// state have been restored correctly following the preview,
		// there will be no update to other citations.
		doh.assertEqual(1, res5.length);
		doh.assertEqual("(Doe, R. Roe)", res5[0][1]);
	},
	function testInitialsChangeWithExternalDelete() {
		var sys, style, res1, res2, res3, res4, res5, data;
		sys = new RhinoTest([ITEM1, ITEM2, ITEM3]);
		style =new CSL.Engine(sys,mycsl);
		[data, res1] = style.processCitationCluster(CITATION1, [], []);
		// print("~~~~~~~~~~~~~~~~~~~~~~ print1 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		[data, res2] = style.processCitationCluster(CITATION2, [["CITATION-1", 1]], []);
		// names rendered are:
		//   C1i1: John Doe, Jane Roe
		//   C2i2: John Doe, Richard Roe
		//   C4i3: Harvey Wallbanger, Horatio Smith
		// print("~~~~~~~~~~~~~~~~~~~~~~ print2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		res3 = style.processCitationCluster(CITATION4, [["CITATION-1", 1], ["CITATION-2", 2]], []);


		// names rendered are:
        //   C1i1: John Doe, Jane Roe
		//   C5i1: John Doe, Jane Roe
		//   C4i3: Harvey Wallbanger, Horatio Smith
		// print("~~~~~~~~~~~~~~~~~~~~~~ preview ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		res4 = style.previewCitationCluster(CITATION5, [["CITATION-1", 1]], [["CITATION-4", 3]], "html", CITATION2);
		doh.assertEqual("(Doe, Roe)", res4);



		// changes to:
		//   C2i2: John Doe, Richard Roe
		//   C4i3: Harvey Wallbanger, Horatio Smith
		// print("~~~~~~~~~~~~~~~~~~~~~~ print3 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		[data, res5] = style.processCitationCluster(CITATION2x, [], [["CITATION-4", 2]]);
		//doh.assertEqual(1, res5);
		doh.assertEqual("(Doe, Roe)", res5[0][1]);

	},
	function testFirstCiteTwoMatchingRefs() {
		var sys = new RhinoTest([ITEM1, ITEM3, ITEM4]);
		var style = new CSL.Engine(sys,mycsl);
		var res = style.previewCitationCluster(CITATION6, [], [], "html");
		doh.assertEqual("(Wallbanger, Smith 1999; Wallbanger, Smith 1999)", res);
	},
	function testFirstCiteYearSuffixPreviewEditPreviewEdit() {
		var res;
		var sys = new RhinoTest([ITEM5, ITEM6]);
		var style = new CSL.Engine(sys,mycsl2);
		[date, res] = style.processCitationCluster(CITATION7, [], [], "html");
		res = style.previewCitationCluster(CITATION7, [], [], "html");
		doh.assertEqual("(Gershwin 1999a; Gershwin 1999b)", res);
		[date, res] = style.processCitationCluster(CITATION7, [], [], "html");
		doh.assertEqual("(Gershwin 1999a; Gershwin 1999b)", res[0][1]);
	}
]);
