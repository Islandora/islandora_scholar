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

/*global CSL: true */

CSL.Engine = function (sys, style, lang, forceLang) {
    var attrs, langspec, localexml, locale;
    this.processor_version = "1.0.250";
    this.csl_version = "1.0";
    this.sys = sys;
    this.sys.xml = new CSL.System.Xml.Parsing();
    if ("string" !== typeof style) {
        style = "";
    }
    if (CSL.getAbbreviation) {
        this.sys.getAbbreviation = CSL.getAbbreviation;
    }
    this.parallel = new CSL.Parallel(this);
    //this.parallel.use_parallels = true;

    this.transform = new CSL.Transform(this);
    // true or false
    this.setParseNames = function (val) {
        this.opt['parse-names'] = val;
    };
    
    this.opt = new CSL.Engine.Opt();
    this.tmp = new CSL.Engine.Tmp();
    this.build = new CSL.Engine.Build();
    this.fun = new CSL.Engine.Fun();
    this.configure = new CSL.Engine.Configure();
    // Build citation before citation_sort in order to pick up
    // state.opt.update_mode, needed it determine whether
    // a grouped sort should be performed.
    this.citation_sort = new CSL.Engine.CitationSort();
    this.bibliography_sort = new CSL.Engine.BibliographySort();
    this.citation = new CSL.Engine.Citation(this);
    this.bibliography = new CSL.Engine.Bibliography();

    this.output = new CSL.Output.Queue(this);

    //this.render = new CSL.Render(this);
    //
    // This latter queue is used for formatting date chunks
    // before they are folded back into the main queue.
    //
    this.dateput = new CSL.Output.Queue(this);

    this.cslXml = this.sys.xml.makeXml(style);
    this.sys.xml.addMissingNameNodes(this.cslXml);
    this.sys.xml.addInstitutionNodes(this.cslXml);
    this.sys.xml.insertPublisherAndPlace(this.cslXml);
    this.sys.xml.flagDateMacros(this.cslXml);
    //
    // Note for posterity: tried manipulating the XML here to insert
    // a list of the upcoming date-part names.  The object is apparently
    // read-only.  Don't know why, can't find any reference to this
    // problem on the Net (other than casual mentions that it works,
    // which it doesn't, at least in Rhino).  Turning to add this info
    // in the compile phase, in the flattened version of the style,
    // which should be simpler anyway.
    //
    attrs = this.sys.xml.attributes(this.cslXml);
    if ("undefined" === typeof attrs["@sort-separator"]) {
        this.sys.xml.setAttribute(this.cslXml, "sort-separator", ", ");
    }
    //if ("undefined" === typeof attrs["@name-delimiter"]) {
    //    this.sys.xml.setAttribute(this.cslXml, "name-delimiter", ", ");
    //}

    this.opt["initialize-with-hyphen"] = true;

    //
    // implicit default, "en"
    //
    // We need to take this in layered functions.  This function goes
    // once, right here, with the lang argument.  It calls a function
    // that resolves the lang argument into a full two-part language
    // entry.  It calls a function that (1) checks to see if
    // the two-part target language is available on the CSL.locale
    // branch, and (2) if not, sets from ultimate default, then (3) from
    // single-term language (if available), then (4) from two-term language.
    // It does this all twice, once for locale files, then for locales inside
    // the style file.
    //
    // So functions needed are ... ?
    //
    // setLocale(rawLang) [top-level]
    //   localeResolve(rawLang)
    //   [function below is not run for external locales
    //   if two-part lang exists in CSL.locale; it is always
    //   run for in-CSL locale data, if it exists.]
    //   localeSetFromXml(lang,localeHandler)
    //
    // (getHandler arg is a function with two methods, one to
    // get XML from disk file, or from CSL, and another to store
    // locale data on CSL.locale or on state.locale, depending
    // on its source.)
    //
    // (note that getLocale must fail gracefully
    // if no locale of the exact lang in the first
    // arg is available.)
    //
    // (note that this will require that the hard-coded locale
    // be recorded on CSL.locale, and that ephemeral locale
    // overlay data bet recorded on state.locale, and that
    // getTerm implement overlay behavior.)
    //

    // Refactored locale resolution
    //
    // (1) Get three locale strings 
    //     -- default-locale (stripped)
    //     -- processor-locale
    //     -- en_US
    
    this.setStyleAttributes();

    this.opt.xclass = sys.xml.getAttributeValue(this.cslXml, "class");
    this.opt.styleID = this.sys.xml.getStyleId(this.cslXml);


    // We seem to have two language specs flying around:
    //   this.opt["default-locale"], and this.opt.lang
    // Keeping them aligned for safety's sake, pending
    // eventual cleanup.
    if (lang) {
        lang = lang.replace("_", "-");
    }
    if (this.opt["default-locale"][0]) {
        this.opt["default-locale"][0] = this.opt["default-locale"][0].replace("_", "-");
    }
    if (lang && forceLang) {
        this.opt["default-locale"] = [lang];
    }
    if (lang && !forceLang && this.opt["default-locale"][0]) {
        lang = this.opt["default-locale"][0];
    }
    if (this.opt["default-locale"].length === 0) {
        if (!lang) {
            lang = "en-US";
        }
        this.opt["default-locale"].push("en-US");
    }
    if (!lang) {
        lang = this.opt["default-locale"][0];
    }
    langspec = CSL.localeResolve(lang);
    this.opt.lang = langspec.best;
    this.opt["default-locale"][0] = langspec.best;
    this.locale = {};
    this.localeConfigure(langspec);

    this.registry = new CSL.Registry(this);

    this.buildTokenLists("citation");
    this.buildTokenLists("bibliography");
    this.configureTokenLists();

    this.disambiguate = new CSL.Disambiguation(this);

    this.splice_delimiter = false;

    //
    // date parser
    //
    this.fun.dateparser = new CSL.DateParser();
    //
    // flip-flopper for inline markup
    //
    this.fun.flipflopper = new CSL.Util.FlipFlopper(this);
    //
    // utility functions for quotes
    //
    this.setCloseQuotesArray();
    //
    // configure ordinal numbers generator
    //
    this.fun.ordinalizer.init(this);
    //
    // configure long ordinal numbers generator
    //
    this.fun.long_ordinalizer.init(this);
    //
    // set up page mangler
    //
    this.fun.page_mangler = CSL.Util.PageRangeMangler.getFunction(this);

    this.setOutputFormat("html");
};

CSL.Engine.prototype.setCloseQuotesArray = function () {
    var ret;
    ret = [];
    ret.push(this.getTerm("close-quote"));
    ret.push(this.getTerm("close-inner-quote"));
    ret.push('"');
    ret.push("'");
    this.opt.close_quotes_array = ret;
};

CSL.Engine.prototype.buildTokenLists = function (area) {
    var area_nodes, navi;
    area_nodes = this.sys.xml.getNodesByName(this.cslXml, area);
    if (!this.sys.xml.getNodeValue(area_nodes)) {
        return;
    }
    navi = new this.getNavi(this, area_nodes);
    this.build.area = area;
    CSL.buildStyle.call(this, navi);
};

CSL.Engine.prototype.setStyleAttributes = function () {
    var dummy, attr, key, attributes, attrname;
    dummy = {};
    dummy.name = this.sys.xml.nodename(this.cslXml);
    //
    // Xml: more of it
    //
    attributes = this.sys.xml.attributes(this.cslXml);
    for (attrname in attributes) {
        if (attributes.hasOwnProperty(attrname)) {
            // attr = attributes[key];
            CSL.Attributes[attrname].call(dummy, this, attributes[attrname]);
        }
    }
};

CSL.buildStyle  = function (navi) {
    if (navi.getkids()) {
        CSL.buildStyle.call(this, navi);
    } else {
        if (navi.getbro()) {
            CSL.buildStyle.call(this, navi);
        } else {
            while (navi.nodeList.length > 1) {
                if (navi.remember()) {
                    CSL.buildStyle.call(this, navi);
                }
            }
        }
    }
};


CSL.Engine.prototype.getNavi = function (state, myxml) {
    this.sys = state.sys;
    this.state = state;
    this.nodeList = [];
    this.nodeList.push([0, myxml]);
    this.depth = 0;
};


CSL.Engine.prototype.getNavi.prototype.remember = function () {
    var node;
    this.depth += -1;
    this.nodeList.pop();
    // closing node, process result of children
    node = this.nodeList[this.depth][1][(this.nodeList[this.depth][0])];
    CSL.XmlToToken.call(node, this.state, CSL.END);
    return this.getbro();
};


CSL.Engine.prototype.getNavi.prototype.getbro = function () {
    var sneakpeek;
    sneakpeek = this.nodeList[this.depth][1][(this.nodeList[this.depth][0] + 1)];
    if (sneakpeek) {
        this.nodeList[this.depth][0] += 1;
        return true;
    } else {
        return false;
    }
};


CSL.Engine.prototype.getNavi.prototype.getkids = function () {
    var currnode, sneakpeek, pos, node, len;
    currnode = this.nodeList[this.depth][1][this.nodeList[this.depth][0]];
    sneakpeek = this.sys.xml.children(currnode);
    //var sneakpeek = currnode.children();
    if (this.sys.xml.numberofnodes(sneakpeek) === 0) {
        // singleton, process immediately
        CSL.XmlToToken.call(currnode, this.state, CSL.SINGLETON);
        return false;
    } else {
        // if there are children, check for date nodes and
        // convert if appropriate
//        for (pos = 0, len = sneakpeek.length; pos < len; pos += 1) {
        for (pos in sneakpeek) {
            //
            // Aha!  If we're to be cross-platform, we can't
            // rely on E4X type discrimination to identify
            // an XML object.
            //
            //if ("xml" === typeof sneakpeek[pos]) {
            //
            // lie to jslint, for the benefit of Rhino
            //
            if (true) {
                node = sneakpeek[pos];
                if ("date" === this.sys.xml.nodename(node)) {
                    currnode = CSL.Util.fixDateNode.call(this, currnode, pos, node);
                    sneakpeek = this.sys.xml.children(currnode);
                }
            }
            //}
        }
        //
        // if first node of a span, process it, then descend
        CSL.XmlToToken.call(currnode, this.state, CSL.START);
        this.depth += 1;
        this.nodeList.push([0, sneakpeek]);
        return true;
    }
};


CSL.Engine.prototype.getNavi.prototype.getNodeListValue = function () {
    return this.nodeList[this.depth][1];
};

CSL.Engine.prototype.getTerm = function (term, form, plural, gender, mode) {
    if (term && term.match(/[A-Z]/) && term === term.toUpperCase()) {
        CSL.debug("Warning: term key is in uppercase form: "+term);
        term = term.toLowerCase();
    }
    var ret = CSL.Engine.getField(CSL.LOOSE, this.locale[this.opt.lang].terms, term, form, plural, gender);
    // XXXXX Temporary, until locale term is deployed in CSL.
    if (!ret && term === "range-delimiter") {
        ret = "\u2013";
    }
    // XXXXX Not so good: can be neither strict nor tolerant
    if (typeof ret === "undefined" && mode === CSL.STRICT) {
        throw "Error in getTerm: term \"" + term + "\" does not exist.";
    } else if (mode === CSL.TOLERANT) {
        ret = false;
    }
    if (ret) {
        this.tmp.cite_renders_content = true;
    }
    return ret;
};

CSL.Engine.prototype.getDate = function (form) {
    if (this.locale[this.opt.lang].dates[form]) {
        return this.locale[this.opt.lang].dates[form];
    } else {
        return false;
    }
};

CSL.Engine.prototype.getOpt = function (arg) {
    if ("undefined" !== typeof this.locale[this.opt.lang].opts[arg]) {
        return this.locale[this.opt.lang].opts[arg];
    } else {
        return this.locale[this.opt.lang].opts[arg];
    }
};



CSL.Engine.prototype.getVariable = function (Item, varname, form, plural) {
    return CSL.Engine.getField(CSL.LOOSE, Item, varname, form, plural);
};

CSL.Engine.prototype.getDateNum = function (ItemField, partname) {
    if ("undefined" === typeof ItemField) {
        return 0;
    } else {
        return ItemField[partname];
    }
};

CSL.Engine.getField = function (mode, hash, term, form, plural, gender) {
    var ret, forms, f, pos, len, hashterm;
    ret = "";
    if ("undefined" === typeof hash[term]) {
        if (mode === CSL.STRICT) {
            throw "Error in getField: term \"" + term + "\" does not exist.";
        } else {
            return undefined;
        }
    }
    if (gender && hash[term][gender]) {
        hashterm = hash[term][gender];
    } else {
        hashterm = hash[term];
    }
    forms = [];
    if (form === "symbol") {
        forms = ["symbol", "short"];
    } else if (form === "verb-short") {
        forms = ["verb-short", "verb"];
    } else if (form !== "long") {
        forms = [form];
    }
    forms = forms.concat(["long"]);
    len = forms.length;
    for (pos = 0; pos < len; pos += 1) {
        f = forms[pos];
        if ("string" === typeof hashterm || "number" === typeof hashterm) {
            ret = hashterm;
        } else if ("undefined" !== typeof hashterm[f]) {
            if ("string" === typeof hashterm[f] || "number" === typeof hashterm[f]) {
                ret = hashterm[f];
            } else {
                if ("number" === typeof plural) {
                    ret = hashterm[f][plural];
                } else {
                    ret = hashterm[f][0];
                }
            }
            break;
        }
    }
    return ret;
};

CSL.Engine.prototype.configureTokenLists = function () {
    var dateparts_master, area, pos, token, dateparts, part, ppos, pppos, len, llen, lllen;
    //for each (var area in ["citation", "citation_sort", "bibliography","bibliography_sort"]) {
    dateparts_master = ["year", "month", "day"];
    len = CSL.AREAS.length;
    for (pos = 0; pos < len; pos += 1) {
        //var ret = [];
        area = CSL.AREAS[pos];
        llen = this[area].tokens.length - 1;
        for (ppos = llen; ppos > -1; ppos += -1) {
            token = this[area].tokens[ppos];
            //token.pos = ppos;
            //ret.push(token);
            if ("date" === token.name && CSL.END === token.tokentype) {
                dateparts = [];
            }
            if ("date-part" === token.name && token.strings.name) {
                lllen = dateparts_master.length;
                for (pppos = 0; pppos < lllen; pppos += 1) {
                    part = dateparts_master[pppos];
                    if (part === token.strings.name) {
                        dateparts.push(token.strings.name);
                    }
                }
            }
            if ("date" === token.name && CSL.START === token.tokentype) {
                dateparts.reverse();
                token.dateparts = dateparts;
            }
            token.next = (ppos + 1);
            //CSL.debug("setting: "+(pos+1)+" ("+token.name+")");
            if (token.name && CSL.Node[token.name].configure) {
                CSL.Node[token.name].configure.call(token, this, ppos);
            }
        }
        //var offset = "";
        //var lnum = 0;
        //if (area === "citation" && true) {
        //    ret.reverse();
        //    for (ppos = 0, llen = ret.length; ppos < llen; ppos += 1) {
        //        lnum = (ppos);
        //        while ((""+lnum).length < 3) {
        //            lnum = " " + lnum;
        //        }
                //if (ret[ppos].tokentype === CSL.START) {
                //    offset += "  ";
                //} else if (ret[ppos].tokentype === CSL.END) {
                //    offset = offset.slice(0,-2);
                //    print(lnum+offset+"</"+ret[ppos].name+">");
                //} else {
                //    print(lnum+offset+"<"+ret[ppos].name+"/>");
                //}
        //    }
        //}
    }
    this.version = CSL.version;
    return this.state;
};


CSL.Engine.prototype.retrieveItems = function (ids) {
    var ret, pos, len;
    ret = [];
    for (var i = 0, ilen = ids.length; i < ilen; i += 1) {
        ret.push(this.retrieveItem("" + ids[i]));
    }
    return ret;
};

CSL.ITERATION = 0;

// Wrapper for sys.retrieveItem supplied by calling application.
// Adds experimental fields embedded in the note field for
// style development trial and testing purposes.
CSL.Engine.prototype.retrieveItem = function (id) {
    var Item, m, pos, len, mm;

    //Zotero.debug("XXX === ITERATION " + CSL.ITERATION + " "+ id +" ===");
    CSL.ITERATION += 1;

    if (this.registry.generate.genIDs["" + id]) {
        Item = this.registry.generate.items["" + id];
    } else {
        Item = this.sys.retrieveItem("" + id);
    }
    // Mandatory data rescue
    // LEX HACK
    if (Item.type === "bill" && Item.number && !Item.volume && Item.page) {
        Item.volume = Item.number;
        Item.number = undefined;
    }
    // Optional development extension
    if (this.opt.development_extensions.field_hack && Item.note) {
        //Zotero.debug("XXX   (1): "+Item.note);
        m = Item.note.match(CSL.NOTE_FIELDS_REGEXP);
        if (m) {
            //Zotero.debug("XXX   (2)");
            for (pos = 0, len = m.length; pos < len; pos += 1) {
                mm = m[pos].match(CSL.NOTE_FIELD_REGEXP);
                if (!Item[mm[1]] || true) {
                    //Zotero.debug("XXX   (3)");
                    if (CSL.DATE_VARIABLES.indexOf(mm[1]) > -1) {
                        Item[mm[1]] = {raw:mm[2]};
                    } else {
                        //Zotero.debug("XXX   (4)");
                        Item[mm[1]] = mm[2].replace(/^\s+/, "").replace(/\s+$/, "");
                    }
                }
            }
        }
    }
    // not including locator-date
    for (var i = 1, ilen = CSL.DATE_VARIABLES.length; i < ilen; i += 1) {
        var dateobj = Item[CSL.DATE_VARIABLES[i]];
        if (dateobj) {
            // raw date parsing is harmless, but can be disabled if desired
            if (this.opt.development_extensions.raw_date_parsing) {
                if (dateobj.raw) {
                    dateobj = this.fun.dateparser.parse(dateobj.raw);
                }
            }
            Item[CSL.DATE_VARIABLES[i]] = this.dateParseArray(dateobj);
        }
    }
    return Item;
};

CSL.Engine.prototype.setOpt = function (token, name, value) {
    if (token.name === "style") {
        this.opt[name] = value;
    } else if (["citation", "bibliography"].indexOf(token.name) > -1) {
        this[token.name].opt[name] = value;
    } else if (["name-form", "name-delimiter", "names-delimiter"].indexOf(name) === -1) {
        token.strings[name] = value;
    }
};

CSL.Engine.prototype.fixOpt = function (token, name, localname) {
    if (["citation", "bibliography"].indexOf(token.name) > -1) {
        if (! this[token.name].opt[name] && "undefined" !== typeof this.opt[name]) {
            this[token.name].opt[name] = this.opt[name];
        }
    }
    if ("name" === token.name || "names" === token.name) {
        if ("undefined" === typeof token.strings[localname] && "undefined" !== typeof this[this.build.root].opt[name]) {
            token.strings[localname] = this[this.build.root].opt[name];
        }
    }
};
