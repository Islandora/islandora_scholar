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


CSL.DateParser = function () {
    var jiy_list, jiy, jiysplitter, jy, jmd, jr, pos, key, val, yearlast, yearfirst, number, rangesep, fuzzychar, chars, rex, rexdash, rexdashslash, rexslashdash, seasonstrs, seasonrexes, seasonstr, monthstrs, monthstr, mrexes, seasonrex, len, jiymatchstring, jiymatcher;

    // instance object with private constants and a public function.

    // japanese imperial years
    jiy_list = [
        ["\u660E\u6CBB", 1867],
        ["\u5927\u6B63", 1911],
        ["\u662D\u548C", 1925],
        ["\u5E73\u6210", 1988]
    ];
    // years by names (jiy)
    // ... and ...
    // regular expression to trap year name and year (jiysplitter)
    jiy = {};
    len = jiy_list.length;
    for (pos = 0; pos < len; pos += 1) {
        key = jiy_list[pos][0];
        val = jiy_list[pos][1];
        jiy[key] = val;
    }
    jiymatchstring = [];
    for (pos = 0; pos < len; pos += 1) {
        val = jiy_list[pos][0];
        jiymatchstring.push(val);
    }
    jiymatchstring = jiymatchstring.join("|");
    //
    // Oh, dear.  Getting this to work in IE6 is going to
    // be a pain.
    //
    jiysplitter = "(?:" + jiymatchstring + ")(?:[0-9]+)";
    jiysplitter = new RegExp(jiysplitter);
    // for IE6 workaround
    jiymatcher = "(?:" + jiymatchstring + ")(?:[0-9]+)";
    jiymatcher = new RegExp(jiymatcher, "g");
    // japanese regular expression for month or day
    jmd = /(\u6708|\u5E74)/g;
    // japanese regular expression for year
    //jy = /\u65E5$/;
    jy = /\u65E5/;
    // japanese regular expression for range
    jr = /\u301c/g;

    // main parsing regexps
    yearlast = "(?:[?0-9]{1,2}%%NUMD%%){0,2}[?0-9]{4}(?![0-9])";
    yearfirst = "[?0-9]{4}(?:%%NUMD%%[?0-9]{1,2}){0,2}(?![0-9])";
    number = "[?0-9]{1,3}";
    rangesep = "[%%DATED%%]";
    fuzzychar = "[?~]";
    chars = "[a-zA-Z]+";
    rex = "(" + yearfirst + "|" + yearlast + "|" + number + "|" + rangesep + "|" + fuzzychar + "|" + chars + ")";
    rexdash = new RegExp(rex.replace(/%%NUMD%%/g, "-").replace(/%%DATED%%/g, "-"));
    rexdashslash = new RegExp(rex.replace(/%%NUMD%%/g, "-").replace(/%%DATED%%/g, "\/"));
    rexslashdash = new RegExp(rex.replace(/%%NUMD%%/g, "\/").replace(/%%DATED%%/g, "-"));

    // seasons
    //seasonstrs = ["spr", "sum", "fal", "win"];
    // Disabling separate seasons match, moving to
    // month.
    seasonstrs = [];
    seasonrexes = [];
    len = seasonstrs.length;
    for (pos = 0; pos < len; pos += 1) {
        seasonrex = new RegExp(seasonstrs[pos] + ".*");
        seasonrexes.push(seasonrex);
    }

    // months
    this.mstrings = "january february march april may june july august september october november december spring summer fall winter spring summer";
    this.mstrings = this.mstrings.split(" ");

    this.setOrderDayMonth = function() {
        // preferred ordering for numeric dates
        this.monthguess = 1;
        this.dayguess = 0;
    };

    this.setOrderMonthDay = function() {
        // preferred ordering for numeric dates
        this.monthguess = 0;
        this.dayguess = 1;
    };

    this.setOrderMonthDay();

    this.resetMonths = function() {
        var i, ilen, j, jlen;
        // Function to reset months to default.
        this.msets = [];
        for (i = 0, ilen = this.mstrings.length; i < ilen; i += 1) {
            this.msets.push([this.mstrings[i]]);
        }
        this.mabbrevs = [];
        for (i = 0, ilen = this.msets.length; i < ilen; i += 1) {
            // XXX Aha.  Needs to nest here.
            this.mabbrevs.push([]);
            for (j = 0, jlen = this.msets[i].length; j < jlen; j += 1) {
                this.mabbrevs[i].push(this.msets[i][0].slice(0, 3));
            }
        }
        this.mrexes = [];
        for (i = 0, ilen = this.mabbrevs.length; i < ilen; i += 1) {
            this.mrexes.push(new RegExp("(?:" + this.mabbrevs[i].join("|") + ")"));
        }
    };

    this.resetMonths();

    this.addMonths = function(lst) {
        var i, ilen, j, jlen, k, klen, jkey, kkey;
        // Function to extend the month regexes with an additional
        // set of month strings, extending strings as required to
        // resolve ambiguities.

        // Normalize string to list
        if ("string" === typeof lst) {
            lst = lst.split(/\s+/);
        }

        // Check that there are twelve (or sixteen) to add
        if (lst.length !== 12 && lst.length !== 16) {
            CSL.debug("month [+season] list of "+lst.length+", expected 12 or 16. Ignoring.");
            return;
        }

        // Extend as necessary to resolve ambiguities
        var othermatch = [];
        var thismatch = [];
        // For each new month string ...
        for (i = 0, ilen = lst.length; i < ilen; i += 1) {
            // Compare with each existing abbreviation and ...
            var abbrevlen = false;
            var skip = false;
            var insert = 3;
            var extend = {};
            for (j = 0, jlen = this.mabbrevs.length; j < jlen; j += 1) {
                // Set default abbrevlen
                extend[j] = {};
                if (j === i) {
                    // Mark for skipping if same as an existing abbreviation of same month
                    for (k = 0, klen = this.mabbrevs[i].length; k < klen; k += 1) {
                        if (this.mabbrevs[i][k] === lst[i].slice(0, this.mabbrevs[i][k].length)) {
                            skip = true;
                            break;
                        }
                    }
                } else {
                    // Mark for extending if same as existing abbreviation of any other month
                    for (k = 0, klen = this.mabbrevs[j].length; k < klen; k += 1) {
                        abbrevlen = this.mabbrevs[j][k].length;
                        if (this.mabbrevs[j][k] === lst[i].slice(0, abbrevlen)) {
                            while (this.msets[j][k].slice(0, abbrevlen) === lst[i].slice(0, abbrevlen)) {
                                // Abort when full length is hit, otherwise extend
                                if (abbrevlen > lst[i].length || abbrevlen > this.msets[j][k].length) {
                                    CSL.debug("unable to disambiguate month string in date parser: "+lst[i]);
                                    break;
                                } else {
                                    // Mark both new entry and existing abbrev for extension
                                    abbrevlen += 1;
                                }
                            }
                            insert = abbrevlen;
                            extend[j][k] = abbrevlen;
                        }
                    }
                }
                for (jkey in extend) {
                    if (extend.hasOwnProperty(jkey)) {
                        for (kkey in extend[jkey]) {
                            if (extend[jkey].hasOwnProperty(kkey)) {
                                abbrevlen = extend[jkey][kkey];
                                jkey = parseInt(jkey, 10);
                                kkey = parseInt(kkey, 10);
                                this.mabbrevs[jkey][kkey] = this.msets[jkey][kkey].slice(0, abbrevlen);
                            }
                        }
                    }
                }
            }
            // Insert here
            if (!skip) {
                this.msets[i].push(lst[i]);
                this.mabbrevs[i].push(lst[i].slice(0, insert));
            }
        }

        // Compose
        this.mrexes = [];
        for (i = 0, ilen = this.mabbrevs.length; i < ilen; i += 1) {
            this.mrexes.push(new RegExp("(?:" + this.mabbrevs[i].join("|") + ")"));
        }
    };

    this.parse = function (txt) {
        var slash, dash, lst, l, m, number, note, thedate, slashcount, range_delim, date_delim, ret, delim_pos, delims, isrange, suff, date, breakme, item, delim, element, mm, slst, mmpos, i, ilen, j, jlen, k, klen;
        //
        // Normalize the format and the year if it's a Japanese date
        //
        m = txt.match(jmd);
        if (m) {
            txt = txt.replace(/\s+/, "", "g");
            txt = txt.replace(jy, "", "g");
            txt = txt.replace(jmd, "-", "g");
            txt = txt.replace(jr, "/", "g");
            txt = txt.replace("-/", "/", "g");
            txt = txt.replace(/-$/,"", "g");

            // Not IE6 safe, applying tortuous workaround
            slst = txt.split(jiysplitter);
            lst = [];
            mm = txt.match(jiymatcher);
            var mmx = [];
            for (pos = 0, len = mm.length; pos < len; pos += 1) {
                mmx = mmx.concat(mm[pos].match(/([^0-9]+)([0-9]+)/).slice(1));
            }
            for (pos = 0, len = slst.length; pos < len; pos += 1) {
                lst.push(slst[pos]);
                if (pos !== (len - 1)) {
                    mmpos = (pos * 2);
                    lst.push(mmx[mmpos]);
                    lst.push(mmx[mmpos + 1]);
                }
            }
            // workaround duly applied, this now works
            l = lst.length;
            for    (pos = 1; pos < l; pos += 3) {
                lst[pos + 1] = jiy[lst[pos]] + parseInt(lst[pos + 1], 10);
                lst[pos] = "";
            }
            txt = lst.join("");
            txt = txt.replace(/\s*-\s*$/, "").replace(/\s*-\s*\//, "/");
            //
            // normalize date and identify delimiters
            //
            txt = txt.replace(/\.\s*$/, "");

            // not sure what this is meant to do
            txt = txt.replace(/\.(?! )/, "");
            slash = txt.indexOf("/");
            dash = txt.indexOf("-");
        }
        // drop punctuation from a.d., b.c.
        txt = txt.replace(/([A-Za-z])\./g, "$1");

        number = "";
        note = "";
        thedate = {};
        if (txt.slice(0, 1) === "\"" && txt.slice(-1) === "\"") {
            thedate.literal = txt.slice(1, -1);
            return thedate;
        }
        if (slash > -1 && dash > -1) {
            slashcount = txt.split("/");
            if (slashcount.length > 3) {
                range_delim = "-";
                date_delim = "/";
                lst = txt.split(rexslashdash);
            } else {
                range_delim = "/";
                date_delim = "-";
                lst = txt.split(rexdashslash);
            }
        } else {
            txt = txt.replace("/", "-");
            range_delim = "-";
            date_delim = "-";
            lst = txt.split(rexdash);
        }
        ret = [];
        len = lst.length;
        for (pos = 0; pos < len; pos += 1) {
            item = lst[pos];
            m = item.match(/^\s*([\-\/]|[a-zA-Z]+|[\-~?0-9]+)\s*$/);
            if (m) {
                ret.push(m[1]);
            }
        }
        //
        // Phase 2
        //
        delim_pos = ret.indexOf(range_delim);
        delims = [];
        isrange = false;
        if (delim_pos > -1) {
            delims.push([0, delim_pos]);
            delims.push([(delim_pos + 1), ret.length]);
            isrange = true;
        } else {
            delims.push([0, ret.length]);
        }
        //
        // For each side of a range divide ...
        //
        suff = "";
        for (i = 0, ilen = delims.length; i < ilen; i += 1) {
            delim = delims[i];
            //
            // Process each element ...
            //
            date = ret.slice(delim[0], delim[1]);
            for (j = 0, jlen = date.length; j < jlen; j += 1) {
                element = date[j];
                //
                // If it's a numeric date, process it.
                //
                if (element.indexOf(date_delim) > -1) {
                    this.parseNumericDate(thedate, date_delim, suff, element);
                    continue;
                }
                //
                // If it's an obvious year, record it.
                //
                if (element.match(/[0-9]{4}/)) {
                    thedate[("year" + suff)] = element.replace(/^0*/, "");
                    continue;
                }
                //
                // If it's a month, record it.
                //
                breakme = false;
                for (k = 0, klen = this.mrexes.length; k < klen; k += 1) {
                    if (element.toLocaleLowerCase().match(this.mrexes[k])) {
                        thedate[("month" + suff)] = "" + (parseInt(k, 10) + 1);
                        breakme = true;
                        break;
                    }
                    if (breakme) {
                        continue;
                    }
                    //
                    // If it's a number, make a note of it
                    //
                    if (element.match(/^[0-9]+$/)) {
                        number = parseInt(element, 10);
                    }
                    //
                    // If it's a BC or AD marker, make a year of
                    // any note.  Separate, reverse the sign of the year
                    // if it's BC.
                    //
                    if (element.toLocaleLowerCase().match(/^bc/) && number) {
                        thedate[("year" + suff)] = "" + (number * -1);
                        number = "";
                        continue;
                    }
                    if (element.toLocaleLowerCase().match(/^ad/) && number) {
                        thedate[("year" + suff)] = "" + number;
                        number = "";
                        continue;
                    }
                }
                //
                // If it's a season, record it.
                //
                breakme = false;
                for (k = 0, klen = seasonrexes.length; k < klen; k += 1) {
                    if (element.toLocaleLowerCase().match(seasonrexes[k])) {
                        thedate[("season" + suff)] = "" + (parseInt(k, 10) + 1);
                        breakme = true;
                        break;
                    }
                }
                if (breakme) {
                    continue;
                }
                //
                // If it's a fuzzy marker, record it.
                //
                if (element === "~" || element === "?" || element === "c" || element.match(/^cir/)) {
                    thedate.circa = "" + 1;
                    continue;
                }
                //
                // If it's cruft, make a note of it
                //
                if (element.toLocaleLowerCase().match(/(?:mic|tri|hil|eas)/) && !thedate[("season" + suff)]) {
                    note = element;
                    continue;
                }
            }
            //
            // If at the end of the string there's still a note
            // hanging around, make a day of it.
            //
            if (number) {
                thedate[("day" + suff)] = number;
                number = "";
            }
            //
            // If at the end of the string there's cruft lying
            // around, and the season field is empty, put the
            // cruft there.
            //
            if (note && !thedate[("season" + suff)]) {
                thedate[("season" + suff)] = note;
                note = "";
            }
            suff = "_end";
        }
        //
        // update any missing elements on each side of the divide
        // from the other
        //
        if (isrange) {
            
            for (j = 0, jlen = CSL.DATE_PARTS_ALL.length; j < jlen; j += 1) {
                item = CSL.DATE_PARTS_ALL[j];
                if (thedate[item] && !thedate[(item + "_end")]) {
                    thedate[(item + "_end")] = thedate[item];
                } else if (!thedate[item] && thedate[(item + "_end")]) {
                    thedate[item] = thedate[(item + "_end")];
                }
            }
        }
        //
        // Replace months with season if appropriate.  
        // 
        //
        // If there's no year, it's a failure; pass through the literal
        //
        if (!thedate.year) {
            thedate = { "literal": txt };
        }
        if (this.use_array) {
            this.toArray(thedate);            
        }
        return thedate;
    };

    this.returnAsArray = function () {
        this.use_array = true;
    };

    this.returnAsKeys = function () {
        this.use_array = false;
    };

    this.toArray = function (thedate) {
        var i, ilen, part;
        thedate["date-parts"] = [];
        thedate["date-parts"].push([]);
        var slicelen = 0;
        for (i = 0, ilen = 3; i < ilen; i += 1) {
            part = ["year", "month", "day"][i];
            if (!thedate[part]) {
                break;
            }
            slicelen += 1;
            thedate["date-parts"][0].push(thedate[part]);
            delete thedate[part];
        }
        for (i = 0, ilen = slicelen; i < ilen; i += 1) {
            part = ["year_end", "month_end", "day_end"][i];
            if (thedate[part] && thedate["date-parts"].length === 1) {
                thedate["date-parts"].push([]);
            }
            thedate["date-parts"][1].push(thedate[part]);
            delete thedate[part];
        }
    };

    this.parseNumericDate = function (ret, delim, suff, txt) {
        var lst, i, ilen;
        lst = txt.split(delim);
        
        for (i = 0, ilen = lst.length; i < ilen; i += 1) {
            if (lst[i].length === 4) {
                ret[("year" + suff)] = lst[i].replace(/^0*/, "");
                if (!i) {
                    lst = lst.slice(1);
                } else {
                    lst = lst.slice(0, i);
                }
                break;
            }
        }
        // comment
        
        for (i = 0, ilen = lst.length; i < ilen; i += 1) {
            lst[i] = parseInt(lst[i], 10);
        }
        //
        // month and day parse
        //
        if (lst.length === 1) {
            ret[("month" + suff)] = "" + lst[0];
        } else if (lst.length === 2) {
            if (lst[this.monthguess] > 12) {
                ret[("month" + suff)] = "" + lst[this.dayguess];
                ret[("day" + suff)] = "" + lst[this.monthguess];
            } else {
                ret[("month" + suff)] = "" + lst[this.monthguess];
                ret[("day" + suff)] = "" + lst[this.dayguess];
            }
        }
    };
};
