/*
 * Copyright (c) 2009 and 2010 Frank G. Bennett, Jr. All Rights Reserved.
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
 * Copyright (c) 2009 and 2010 Frank G. Bennett, Jr. All Rights Reserved.
 */

dojo.provide("citeproc_js.load_all_styles");


var tryStyle = function(style){
	try {
		var sty = readFile("all-styles/csl/"+style+".csl");
		if (!sty){
			throw "Did not find style file: all-styles/csl/"+style+".csl";
		}
		var builder = new CSL.Core.Build(sty);
		var res = builder.build();
	} catch(e) {
		CSL.debug("oops: "+e);
	}
	return res;
}

doh.register("citeproc_js.load_styles", [
 function(){
		var res = tryStyle("a251");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("aacr");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("aag");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("acm-sigchi-proceedings");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("acs-chemical-biology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("acta-materialia");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("advanced-engineering-materials");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("agu");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("aip");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("aja-author-date");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("all");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("ama");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("amerantiq-draft4");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("american-chemical-society");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("american-journal-of-physical-anthropology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("american-journal-of-psychiatry");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("american-phytopathological-society");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("anal-chim-acta");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("analytic-chemistry");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("ann-bot");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("antonie-van-leeuwenhoek");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("apa");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("apsa");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("aps");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("art-history");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("asa");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("asce");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("asme");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("asm-journals");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("australian-historical-studies");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("australian-legal");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("basic-and-applied-ecology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("besj");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("bibtex");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("biochemistry");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("bioconjugate-chemistry");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("bioinformatics");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("blood");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("bluebook-law_review");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("bmc-bioinformatics");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("brain");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("briefings_in_bioinformatics");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("british_psychological_society");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("cell-calcium");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("cell");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("centarus");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chemical-research-in-toxicology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-annotated-bibliography");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-author-date");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-author-date_de");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-fullnote-bibliography");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-library-list");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-note-bibliography");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-note");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-note-no-ibid");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("chicago-quick-copy");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("Chinese-Building-Structure");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("Chinese-Std-GBT7714-Numeric");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("clinical-cancer-research");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("cse");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("cshlp");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("current-biology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("din-1505-2");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("ecology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("elsevier-without-titles");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("elsevier-with-titles");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("febs");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("fems");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("genesdev");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("genomebiology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("geomorphology");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("harvard1");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("harvard1de");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("harvard2");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("harvard3");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("harvard7de");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("harvard-leeds-met");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("history-journal");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("history-theory");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("ieee");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("immunity");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("international-journal-of-hydrogen-energy");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("inter-ro");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("iso690-author-date");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("jacs");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("jbc");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("jeb");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("jimmunol");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("journal-neuroscience");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("journal-neurosurgery");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("journal-of-archaeological-science");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("journal-of-the-american-dietetic-association");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("journal-of-the-royal-anthropological-institute");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("law1_de");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("lichenologist");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("mcgill-legal");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("metabolic-eng");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("metallurgical-and-materials-transactions");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("meteoritics");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("mhra");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("mhra_note_without_bibliography");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("microscopy-and-microanalysis");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("mla");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("mla-underline");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("mycol-res");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("naa-simple");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("nature");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("neuroreport");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("new-phytol");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("nlm");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("nuc-acid-res");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("oecologia");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("oikos");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("ou-harvard");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("ou-harvard-numeric");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("oxford-art");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("plos");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("pnas");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("progress-in-materials-science");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("rna");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("ro-humanities");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("royal-society-chemistry");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("sbl-fullnote-bibliography");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("science");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("sp-legal");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("tah-gkw");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("tah-leviathan");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("tah-soz");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("test");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("turabian");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("urban_studies");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("vancouver");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("vancouver-superscript");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("vienna-legal");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("waterresearch");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("water-sciences-tech");
		doh.assertTrue( res );
	},
 function(){
		var res = tryStyle("yeast");
		doh.assertTrue( res );
	},
]);
