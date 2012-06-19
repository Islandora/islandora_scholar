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

var StdRhinoTest = function(myname,custom){
    this.myname = myname;
    this._cache = {};
    // "default" now actually means something: the default jurisdiction.
    this._acache = {};
    this._acache["default"] = new CSL.AbbreviationSegments();
    this._ids = [];
    if (myname){
        var test;
        //if ("undefined" != typeof custom && custom == "custom"){
        //    test = readFile("./tests/custom/" + myname + ".json", "UTF-8");
        //} else if ("undefined" != typeof custom && custom == "local"){
        //    test = readFile("./tests/local/machines/" + myname + ".json", "UTF-8");
        //} else {
        //    test = readFile("./tests/std/machines/" + myname + ".json", "UTF-8");
        //}
        test = readFile("./tests/fixtures/run/machines/" + myname + ".json", "UTF-8");
        eval( "this.test = "+test);
        this.result = this.test.result;
        this._setCache();
    }
};

//
// Retrieve properly composed item from phoney database.
// (Deployments must provide an instance object with
// this method.)
//
StdRhinoTest.prototype.retrieveItem = function(id){
    return this._cache[id];
};

StdRhinoTest.prototype.getAbbreviation = function(dummyListNameVar, obj, jurisdiction, category, key){
    var newkey = key;
    if (!this._acache[jurisdiction]) {
        this._acache[jurisdiction] = new CSL.AbbreviationSegments();
    }
    if (!obj[jurisdiction]) {
        obj[jurisdiction] = new CSL.AbbreviationSegments();
    }    
    var jurisdictions = ["default"];
    if (jurisdiction !== "default") {
        jurisdictions.push(jurisdiction);
    }
    jurisdictions.reverse();
    var haveHit = false;
    for (var i = 0, ilen = jurisdictions.length; i < ilen; i += 1) {
        var myjurisdiction = jurisdictions[i];
        if (this._acache[myjurisdiction][category][key]) {
            obj[myjurisdiction][category][key] = this._acache[myjurisdiction][category][key];
            jurisdiction = myjurisdiction;
            haveHit = true;
            break;
        }
    }
    if (!haveHit) {
        for (var i = 0, ilen = jurisdictions.length; i < ilen; i += 1) {
            if (["container-title", "collection-title", "number"].indexOf(category) > -1) {
                // Let's just be inefficient
                for (var phrase in this._acache[jurisdictions[i]]["container-phrase"]) {
                    var newphrase = this._acache[jurisdictions[i]]["container-phrase"][phrase];
                    newkey = newkey.replace(phrase, newphrase);
                }
            } else if (["institution-part", "title", "place"].indexOf(category) > -1) {
                // And again
                for (var phrase in this._acache[jurisdictions[i]]["title-phrase"]) {
                    var newphrase = this._acache[jurisdictions[i]]["title-phrase"][phrase];
                    newkey = newkey.replace(phrase, newphrase);
                }
            }
        }
        if (key !== newkey) {
            obj[jurisdiction][category][key] = newkey;
        } else {
            obj[jurisdiction][category][key] = "";
        }
    }
    return jurisdiction;
};

StdRhinoTest.prototype.addAbbreviation = function(jurisdiction,category,key,val){
    if (!this._acache[jurisdiction]) {
        this._acache[jurisdiction] = new CSL.AbbreviationSegments();
    }
    this._acache[jurisdiction][category][key] = val;
};

//
// Build phoney database.
//
StdRhinoTest.prototype._setCache = function(){
    for each (item in this.test.input){
        this._cache[item.id] = item;
        this._ids.push(item.id);
    }
};


StdRhinoTest.prototype._readTest = function(){
    var test;
    var filename = "std/machines/" + this.myname + ".json";
    //
    // Half of the fix for encoding problem encountered by Sean
    // under OSX.  External strings are _read_ correctly, but an
    // explicit encoding declaration on readFile is needed if
    // they are to be fed to eval.  This may set the implicit
    // UTF-8 binary identifier on the stream, as defined in the
    // ECMAscript specification.  See http://www.ietf.org/rfc/rfc4329.txt
    //
    // Python it's not.  :)
    //
    var teststring = readFile(filename, "UTF-8");
    //
    // Grab test data in an object.
    //
    try {
        eval( "test = "+teststring );
    } catch(e){
        throw e + teststring;
    }
    this.test = test;
};


StdRhinoTest.prototype.run = function(){
    //print("-->"+this.myname);
    var result, data, nosort;
    // print(this.myname);
    var len, pos, ret, id_set, nick;
    ret = new Array();
    this.style = new CSL.Engine(this,this.test.csl);
    var langParams = {
        persons:["translit"],
        institutions:["translit"],
        titles:["translit", "translat"],
        publishers:["translat"],
        places:["translat"]
    }
    this.style.setLangPrefsForCites(langParams);
    if (this.test.abbreviations) {
        for (jurisdiction in this.test.abbreviations) {
            for (field in this.test.abbreviations[jurisdiction]) {
                for (key in this.test.abbreviations[jurisdiction][field]) {
                    this.addAbbreviation(jurisdiction,field,key,this.test.abbreviations[jurisdiction][field][key]);
                }
            }
        }
    }

    if (this.test.mode === "bibliography-nosort") {
        nosort = true;
    }
    if (this.test.bibentries){
        for each (id_set in this.test.bibentries){
            this.style.updateItems(id_set, nosort);
        }
    } else if (!this.test.citations) {
        this.style.updateItems(this._ids, nosort);
    }
    if (!this.test.citation_items && !this.test.citations){
        var citation = [];
        for each (item in this.style.registry.reflist){
            citation.push({"id":item.id});
        }
        this.test.citation_items = [citation];
    }
    var citations = [];
    if (this.test.citation_items){
        for each (var citation in this.test.citation_items){
            // sortCitationCluster(), we hardly knew ya
            // this.style.sortCitationCluster(citation);
            citations.push(this.style.makeCitationCluster(citation));
        }
    } else if (this.test.citations){
        for each (var citation in this.test.citations.slice(0,-1)){
            this.style.processCitationCluster(citation[0],citation[1],citation[2]);
        };
        var citation = this.test.citations.slice(-1)[0];
        [data, result] = this.style.processCitationCluster(citation[0],citation[1],citation[2]);
    };
    var indexMap = new Object();
    for (var pos in result){
        indexMap[""+result[pos][0]] = pos;
    };
    for (var cpos in this.style.registry.citationreg.citationByIndex){
        var citation = this.style.registry.citationreg.citationByIndex[cpos];
        if (indexMap[""+cpos]){
            citations.push(">>["+cpos+"] "+result[indexMap[cpos]][1]);
        } else {
            citations.push("..["+cpos+"] "+this.style.process_CitationCluster.call(this.style,this.style.registry.citationreg.citationByIndex[cpos].sortedItems));
        }
    };
    ret = citations.join("\n");
    if (this.test.mode == "bibliography" || this.test.mode == "bibliography-nosort"){
        if (this.test.bibsection){
            var ret = this.style.makeBibliography(this.test.bibsection);
        } else {
            var ret = this.style.makeBibliography();
        }
        ret = ret[0]["bibstart"] + ret[1].join("") + ret[0]["bibend"];
    } else if (this.test.mode == "bibliography-header"){
        var obj = this.style.makeBibliography()[0];
        var lst = [];
        for (var key in obj) {
            var keyval = [];
            keyval.push(key);
            keyval.push(obj[key]);
            lst.push(keyval);
        }
        lst.sort(
            function (a, b) {
                if (a > b) {
                    return 1;
                } else if (a < b) {
                    return -1;
                } else {
                    return 0;
                }
            }
        );
        ret = "";
        for (pos = 0, len = lst.length; pos < len; pos += 1) {
            ret += lst[pos][0] + ": " + lst[pos][1] + "\n";
        }
        ret = ret.replace(/^\s+/,"").replace(/\s+$/,"");
    }
    if (this.test.mode !== "bibliography" && this.test.mode !== "citation" && this.test.mode !== "bibliography-header" && this.test.mode != "bibliography-nosort") {
        throw "Invalid mode in test file "+this.myname+": "+this.test.mode;
    }
    return ret;
};

//
// Retrieve locale object from filesystem
// (Deployments must provide an instance object with
// this method.)
//
StdRhinoTest.prototype.retrieveLocale = function(lang){
    var ret = readFile( "./locale/locales-"+lang+".xml", "UTF-8");
    // ret = ret.replace(/\s*<\?[^>]*\?>\s*\n/g, "");
    return ret;
};
