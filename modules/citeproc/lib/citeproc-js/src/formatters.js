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

/**
 * A bundle of handy functions for text processing.
 * <p>Several of these are ripped off from various
 * locations in the Zotero source code.</p>
 * @namespace Toolkit of string functions
 */
CSL.Output.Formatters = {};

CSL.getSafeEscape = function(outputModeOpt, outputArea) {
    if (["bibliography", "citation"].indexOf(outputArea) > -1) {
        return CSL.Output.Formats[outputModeOpt].text_escape;
    } else {
        return function (txt) { return txt; };
    }
};

// See util_substitute.js and queue.js (append) for code supporting
// strip-periods.
//CSL.Output.Formatters.strip_periods = function (state, string) {
//    return string.replace(/\./g, "");
//};


/**
 * A noop that just delivers the string.
 */
CSL.Output.Formatters.passthrough = function (state, string) {
    return string;
};

/**
 * Force all letters in the string to lowercase.
 */
CSL.Output.Formatters.lowercase = function (state, string) {
    var str = CSL.Output.Formatters.doppelString(string, CSL.TAG_USEALL);
    str.string = str.string.toLowerCase();
    return CSL.Output.Formatters.undoppelString(str);
};


/**
 * Force all letters in the string to uppercase.
 */
CSL.Output.Formatters.uppercase = function (state, string) {
    var str = CSL.Output.Formatters.doppelString(string, CSL.TAG_USEALL);
    str.string = str.string.toUpperCase();
    return CSL.Output.Formatters.undoppelString(str);
};


/**
 * Force capitalization of the first letter in the string, leave
 * the rest of the characters untouched.
 */
CSL.Output.Formatters["capitalize-first"] = function (state, string) {
    var str = CSL.Output.Formatters.doppelString(string, CSL.TAG_ESCAPE);
    if (str.string.length) {
        str.string = str.string.slice(0, 1).toUpperCase() + str.string.substr(1);
        return CSL.Output.Formatters.undoppelString(str);
    } else {
        return "";
    }
};


/**
 * Similar to <b>capitalize_first</b>, but force the
 * subsequent characters to lowercase.
 */
CSL.Output.Formatters.sentence = function (state, string) {
    var str = CSL.Output.Formatters.doppelString(string, CSL.TAG_ESCAPE);
    str.string = str.string.slice(0, 1).toUpperCase() + str.string.substr(1).toLowerCase();
    return CSL.Output.Formatters.undoppelString(str);
};


/**
 * Force the first letter of each space-delimited
 * word in the string to uppercase, and force remaining
 * letters to lowercase.  Single characters are forced
 * to uppercase.
 */
CSL.Output.Formatters["capitalize-all"] = function (state, string) {
    var str, strings, len, pos;
    str = CSL.Output.Formatters.doppelString(string, CSL.TAG_ESCAPE);
    strings = str.string.split(" ");
    len = strings.length;
    for (pos = 0; pos < len; pos += 1) {
        if (strings[pos].length > 1) {
            strings[pos] = strings[pos].slice(0, 1).toUpperCase() + strings[pos].substr(1).toLowerCase();
        } else if (strings[pos].length === 1) {
            strings[pos] = strings[pos].toUpperCase();
        }
    }
    str.string = strings.join(" ");
    return CSL.Output.Formatters.undoppelString(str);
};

/**
 * A complex function that attempts to produce a pattern
 * of capitalization appropriate for use in a title.
 * Will not touch words that have some capitalization
 * already.
 */
CSL.Output.Formatters.title = function (state, string) {
    var str, words, isAllUpperCase, newString, lastWordIndex, previousWordIndex, upperCaseVariant, lowerCaseVariant, pos, skip, notfirst, notlast, aftercolon, len, idx, tmp, skipword, ppos, mx, lst, myret;
    var SKIP_WORDS = state.locale[state.opt.lang].opts["skip-words"];
    str = CSL.Output.Formatters.doppelString(string, CSL.TAG_ESCAPE);
    if (!string) {
        return "";
    }

    // split words
    // Workaround for Internet Explorer
    mx = str.string.match(/(\s+)/g);
    lst = str.string.split(/\s+/);
    myret = [lst[0]];
    for (pos = 1, len = lst.length; pos < len; pos += 1) {
        myret.push(mx[pos - 1]);
        myret.push(lst[pos]);
    }
    words = myret.slice();
    isAllUpperCase = str.string.toUpperCase() === string && !str.string.match(/[0-9]/);
    newString = "";
    lastWordIndex = words.length - 1;
    previousWordIndex = -1;
    // Inspect every word individually
    for (pos = 0; pos <= lastWordIndex;  pos += 2) {
        // Word has length, is not a single character, and does not consist only of spaces
        if (words[pos].length !== 0 && words[pos].length !== 1 && !/\s+/.test(words[pos])) {
            upperCaseVariant = words[pos].toUpperCase();
            lowerCaseVariant = words[pos].toLowerCase();
            var totallyskip = false;
            // Full string is not all-uppercase, or string is one word of three characters or less
            if (!isAllUpperCase || (words.length === 1 && words[pos].length < 4)) {
                // This word is all-uppercase
                if (words[pos] === upperCaseVariant) {
                    totallyskip = true;
                }
            }
            // Full string is all-uppercase, or this word is all-lowercase
            if (isAllUpperCase || words[pos] === lowerCaseVariant) {
                skip = false;
                for (var i = 0, ilen = SKIP_WORDS.length; i < ilen; i += 1) {
                    skipword = SKIP_WORDS[i];
                    idx = lowerCaseVariant.indexOf(skipword);
                    if (idx > -1) {
                        tmp = lowerCaseVariant.slice(0, idx) + lowerCaseVariant.slice(idx + skipword.length);
                        if (!tmp.match(/[a-zA-Z]/)) {
                            skip = true;
                        }
                    }
                }
                notfirst = pos !== 0;
                notlast = pos !== lastWordIndex;
                if (words[previousWordIndex]) {
                    aftercolon = words[previousWordIndex].slice(-1) === ":";
                } else {
                    aftercolon = false;
                }
                // Skip if word is all-uppercase, and the full string is in mixed-case
                if (!totallyskip) {
                    // If word is a stop-word, neither first nor last, and does not follow a colon,
                    // force to lowercase
                    // Otherwise capitalize first character
                    if (skip && notfirst && notlast && !aftercolon) {
                        words[pos] = lowerCaseVariant;
                    } else {
                        words[pos] = upperCaseVariant.slice(0, 1) + lowerCaseVariant.substr(1);
                    }
                }
            }
            previousWordIndex = pos;
        }
    }
    str.string = words.join("");
    return CSL.Output.Formatters.undoppelString(str);
};

/*
* Based on a suggestion by Shoji Kajita.
*/
CSL.Output.Formatters.doppelString = function (string, rex) {
    var ret, pos, len;
    ret = {};
    // rex is a function that returns an appropriate array.
    //
    // XXXXX: Does this work in Internet Explorer?
    //
    ret.array = rex(string);
    // ret.array = string.split(rex);
    ret.string = "";
    len = ret.array.length;
    for (pos = 0; pos < len; pos += 2) {
        ret.string += ret.array[pos];
    }
    return ret;
};


CSL.Output.Formatters.undoppelString = function (str) {
    var ret, len, pos;
    ret = "";
    len = str.array.length;
    for (pos = 0; pos < len; pos += 1) {
        if ((pos % 2)) {
            ret += str.array[pos];
        } else {
            ret += str.string.slice(0, str.array[pos].length);
            str.string = str.string.slice(str.array[pos].length);
        }
    }
    return ret;
};


CSL.Output.Formatters.serializeItemAsRdf = function (Item) {
    return "";
};


CSL.Output.Formatters.serializeItemAsRdfA = function (Item) {
    return "";
};


CSL.demoteNoiseWords = function (state, fld) {
    var SKIP_WORDS = state.locale[state.opt.lang].opts["skip-words"];
    if (fld) {
        fld = fld.split(/\s+/);
        fld.reverse();
        var toEnd = [];
        for (var j  = fld.length - 1; j > -1; j += -1) {
            if (SKIP_WORDS.indexOf(fld[j].toLowerCase()) > -1) {
                toEnd.push(fld.pop());
            } else {
                break;
            }
        }
        fld.reverse();
        var start = fld.join(" ");
        var end = toEnd.join(" ");
        fld = [start, end].join(", ");
    }
    return fld;
};
