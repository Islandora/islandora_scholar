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

CSL.Util.padding = function (num) {
    var m = num.match(/\s*(-{0,1}[0-9]+)/);
    if (m) {
        num = parseInt(m[1], 10);
        if (num < 0) {
            num = 99999999999999999999 + num;
        }
        num = "" + num;
        while (num.length < 20) {
            num = "0" + num;
        }
    }
    return num;
};

CSL.Util.LongOrdinalizer = function () {};

CSL.Util.LongOrdinalizer.prototype.init = function (state) {
    this.state = state;
};

CSL.Util.LongOrdinalizer.prototype.format = function (num, gender) {
    if (num < 10) {
        num = "0" + num;
    }
    // Argument true means "loose".
    var ret = CSL.Engine.getField(
        CSL.LOOSE, 
        this.state.locale[this.state.opt.lang].terms,
        "long-ordinal-" + num,
        "long", 
        0, 
        gender
    );
    if (!ret) {
        ret = this.state.fun.ordinalizer.format(num, gender);
    }
    // Probably too optimistic -- what if only renders in _sort?
    this.state.tmp.cite_renders_content = true;
    return ret;
};


CSL.Util.Ordinalizer = function () {};

CSL.Util.Ordinalizer.prototype.init = function (state) {
    this.suffixes = {};
    for (var i = 0, ilen = 3; i < ilen; i += 1) {
        var gender = [undefined, "masculine", "feminine"][i];
        this.suffixes[gender] = [];
        for (var j = 1; j < 5; j += 1) {
            var ordinal = state.getTerm("ordinal-0" + j, "long", false, gender);
            if ("undefined" === typeof ordinal) {
                delete this.suffixes[gender];
                break;
            }
            this.suffixes[gender].push(ordinal);            
        }
    }
};

CSL.Util.Ordinalizer.prototype.format = function (num, gender) {
    var str;
    num = parseInt(num, 10);
    str = num.toString();
    if ((num / 10) % 10 === 1 || (num > 10 && num < 20)) {
        str += this.suffixes[gender][3];
    } else if (num % 10 === 1) {
        str += this.suffixes[gender][0];
    } else if (num % 10 === 2) {
        str += this.suffixes[gender][1];
    } else if (num % 10 === 3) {
        str += this.suffixes[gender][2];
    } else {
        str += this.suffixes[gender][3];
    }
    return str;
};

CSL.Util.Romanizer = function () {};

CSL.Util.Romanizer.prototype.format = function (num) {
    var ret, pos, n, numstr, len;
    ret = "";
    if (num < 6000) {
        numstr = num.toString().split("");
        numstr.reverse();
        pos = 0;
        n = 0;
        len = numstr.length;
        for (pos = 0; pos < len; pos += 1) {
            n = parseInt(numstr[pos], 10);
            ret = CSL.ROMAN_NUMERALS[pos][n] + ret;
        }
    }
    return ret;
};


/**
 * Create a suffix formed from a list of arbitrary characters of arbitrary length.
 * <p>This is a <i>lot</i> harder than it seems.</p>
 */
CSL.Util.Suffixator = function (slist) {
    if (!slist) {
        slist = CSL.SUFFIX_CHARS;
    }
    this.slist = slist.split(",");
};

/**
 * The format method.
 * <p>This method is used in generating ranges.  Every numeric
 * formatter (of which Suffixator is one) must be an instantiated
 * object with such a "format" method.</p>
 */

CSL.Util.Suffixator.prototype.format = function (N) {
    // Many thanks to Avram Lyon for this code, and good
    // riddance to the several functions that it replaces.
    var X;
    N += 1;
    var key = "";
    do {
        X = ((N % 26) === 0) ? 26 : (N % 26);
        key = this.slist[X-1] + key;
        N = (N - X) / 26;
    } while ( N !== 0 );
    return key;
};


CSL.Engine.prototype.processNumber = function (node, ItemObject, variable) {
    var num, m, i, ilen, j, jlen;
    var debug = false;
    num = ItemObject[variable];
    //SNIP-START
    if (debug) {
        print("=== "+variable+": "+num+" ===");
    }
    //SNIP-END

    // This carries value, pluralization and numeric info for use in other contexts.
    // This function does not render.
    this.tmp.shadow_numbers[variable] = {};
    this.tmp.shadow_numbers[variable].values = [];
    this.tmp.shadow_numbers[variable].plural = 0;
    this.tmp.shadow_numbers[variable].numeric = false;
    if ("undefined" !== typeof num) {
        if ("number" === typeof num) {
            num = "" + num;
        }
        // Strip off enclosing quotes, if any. Parsing logic
        // does not depend on them, but we'll strip them if found.
        if (num.slice(0, 1) === '"' && num.slice(-1) === '"') {
            num = num.slice(1, -1);
        }
       
        // Fix up hyphens early
        num = num.replace(/\s*\-\s*/, "\u2013", "g");
 
        if (this.variable === "page-first") {
            m = num.split(/\s*(?:&|,|-)\s*/);
            if (m) {
                num = m[0];
                if (num.match(/[0-9]/)) {
                    this.tmp.shadow_numbers[variable].numeric = true;
                }
            }
        }
        
        // XXX: The attempt at syntactic parsing was crazy.

        // Sequential number blobs should be reserved for year-suffix
        // and year, which may need to collapse during cite
        // composition. For explicit cs:number, we should
        // use simple heuristics to flag multiple
        // values, but respect the user's input format.
        // Ordinals can be applied to pure numeric elements,
        // but that's as far as our fancy processing
        // should go.
        
        // So.
        
        // (1) Split the string on ", ", "\s*[\-\u2013]\s*" and "&".
        // (2) Set the elements one by one, setting pure numbers
        //     as numeric blobs, and everything else as text,
        //     applying "this" for styling to both, with "empty"
        //     styling on the punctuation elements.
        
        var lst = num.split(/(?:,\s+|\s*[\-\u2013]\s*|\s*&\s*)/);
        var m = num.match(/(,\s+|\s*[\-\u2013]\s*|\s*&\s*)/g);
        var elements = [];
        for (var i = 0, ilen = lst.length - 1; i < ilen; i += 1) {
            elements.push(lst[i]);
            elements.push(m[i]);
        }
        elements.push(lst[lst.length - 1]);
        var count = 0;
        var numeric = true;
        for (var i = 0, ilen = elements.length; i < ilen; i += 1) {
            var odd = ((i%2) === 0);
            if (odd) {
                // Rack up as numeric output if pure numeric, otherwise as text.
                if (elements[i]) {
                    if (elements[i].match(/[0-9]/)) {
                        count = count + 1;
                    }
                    var subelements = elements[i].split(/\s+/);
                    for (var j = 0, jlen = subelements.length; j < jlen; j += 1) {
                        if (!subelements[j].match(/[0-9]/)) {
                            numeric = false;
                        }
                    }
                    // Possibility of redemption
                    if (i === elements.length - 1) {
                        if ((elements.length > 1 || subelements.length > 1)) {
                            var matchterm = this.getTerm(variable, "long");
                            if (matchterm && !subelements[subelements.length - 1].match(/[0-9]/)) {
                                matchterm = matchterm.replace(".", "").toLowerCase().split(/\s+/)[0];
                                if (subelements[subelements.length - 1].slice(0, matchterm.length).toLowerCase() === matchterm) {
                                    // Remove the final word, since it looks like it's just a variable label,
                                    // and force to numeric.
                                    elements[i] = subelements.slice(0, -1).join(" ");
                                    numeric = true;
                                }
                            }
                        }
                    }
                    if (elements[i].match(/^[1-9][0-9]*$/)) {
                        elements[i] = parseInt(elements[i], 10);
                        node.gender = this.opt["noun-genders"][variable];
                        this.tmp.shadow_numbers[variable].values.push(["NumericBlob", elements[i], node]);
                    } else {
                        // XXX Abbreviate elements[i] here, if ... well, always, for the present
                        var str = elements[i];
                        if (this.sys.getAbbreviation) {
                            var jurisdiction = this.transform.loadAbbreviation(ItemObject.jurisdiction, "number", elements[i]);
                            if (this.transform.abbrevs[jurisdiction].number[str]) {
                                str = this.transform.abbrevs[jurisdiction].number[str];
                            }
                        }
                        this.tmp.shadow_numbers[variable].values.push(["Blob", str, node]);
                    }
                }
            } else {
                // Normalize and output as text with "empty" style.
                if (elements[i]) {
                    this.tmp.shadow_numbers[variable].values.push(["Blob", elements[i], undefined]);
                }
            }
        }
        if (num.indexOf(" ") === -1 && num.match(/[0-9]/)) {
            this.tmp.shadow_numbers[variable].numeric = true;
        } else {
             this.tmp.shadow_numbers[variable].numeric = numeric;
       }
        if (count > 1) {
            this.tmp.shadow_numbers[variable].plural = 1;
        }
    }
};
