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

CSL.Node.date = {
    build: function (state, target) {
        var func, date_obj, tok, len, pos, part, dpx, parts, mypos, start, end;
        if (this.tokentype === CSL.START || this.tokentype === CSL.SINGLETON) {
            // used to collect rendered date part names in node_datepart,
            // for passing through to node_key, for use in dates embedded
            // in macros
            state.build.date_parts = [];
            state.build.date_variables = this.variables;
            if (!state.build.extension) {
                CSL.Util.substituteStart.call(this, state, target);
            }
            if (state.build.extension) {
                func = CSL.dateMacroAsSortKey;
            } else {
                func = function (state, Item, item) {
                    var key, dp;
                    state.tmp.element_rendered_ok = false;
                    state.tmp.donesies = [];
                    state.tmp.dateparts = [];
                    dp = [];
                    //if (this.variables.length && Item[this.variables[0]]){
                    if (this.variables.length
                        && !(state.tmp.just_looking
                             && this.variables[0] !== "issued")) {
                        
                        state.parallel.StartVariable(this.variables[0]);
                        date_obj = Item[this.variables[0]];
                        if ("undefined" === typeof date_obj) {
                            date_obj = {"date-parts": [[0]] };
                            if (state.opt.development_extensions.locator_date_and_revision) {
                                if (item && this.variables[0] === "locator-date" && item["locator-date"]) {
                                    date_obj = item["locator-date"];
                                }
                            }
                        }
                        state.tmp.date_object = date_obj;
                        //
                        // Call a function here to analyze the
                        // data and set the name of the date-part that
                        // should collapse for this range, if any.
                        //
                        // (1) build a filtered list, in y-m-d order,
                        // consisting only of items that are (a) in the
                        // date-parts and (b) in the *_end data.
                        // (note to self: remember that season is a
                        // fallback var when month and day are empty)
                        
                        //if ("undefined" === typeof this.dateparts) {
                        //    this.dateparts = ["year", "month", "day"];
                        //}
                        len = this.dateparts.length;
                        for (pos = 0; pos < len; pos += 1) {
                            part = this.dateparts[pos];
                            if ("undefined" !== typeof state.tmp.date_object[(part +  "_end")]) {
                                dp.push(part);
                            } else if (part === "month" && "undefined" !== typeof state.tmp.date_object.season_end) {
                                dp.push(part);
                            }
                        }
                        dpx = [];
                        parts = ["year", "month", "day"];
                        len = parts.length;
                        for (pos = 0; pos < len; pos += 1) {
                            if (dp.indexOf(parts[pos]) > -1) {
                                dpx.push(parts[pos]);
                            }
                        }
                        dp = dpx.slice();
                        // Suppress the year if we're not sorting, and
                        // it's the same as the volume, and we would render
                        // only the year, with not month or day.
                        // Needed for English-style case cites.  Here's hoping it
                        // doesn't have side effects.
                        if (!state.tmp.extension && ("" + Item.volume) === "" + state.tmp.date_object.year && this.dateparts.length === 1 && this.dateparts[0] === "year") {
                            for (key in state.tmp.date_object) {
                                if (state.tmp.date_object.hasOwnProperty(key)) {
                                    if (key.slice(0, 4) === "year" && state.tmp.citeblob.can_suppress_identical_year) {
                                        delete state.tmp.date_object[key];
                                    }
                                }
                            }
                        }
                        //
                        // (2) Reverse the list and step through in
                        // reverse order, popping each item if the
                        // primary and *_end data match.
                        mypos = 2;
                        len = dp.length;
                        for (pos = 0; pos < len; pos += 1) {
                            part = dp[pos];
                            start = state.tmp.date_object[part];
                            end = state.tmp.date_object[(part + "_end")];
                            if (start !== end) {
                                mypos = pos;
                                break;
                            }
                        }
                        
                        //
                        // (3) When finished, the first item in the
                        // list, if any, is the date-part where
                        // the collapse should occur.

                        // XXXXX: was that it?
                        state.tmp.date_collapse_at = dp.slice(mypos);
                        //
                        // The collapse itself will be done by appending
                        // string output for the date, less suffix,
                        // placing a delimiter on output, then then
                        // doing the *_end of the range, dropping only
                        // the prefix.  That should give us concise expressions
                        // of ranges.
                        //
                        // Numeric dates should not collapse, though,
                        // and should probably use a slash delimiter.
                        // Scope for configurability will remain (all over
                        // the place), but this will do to get this feature
                        // started.
                        //
                    } else {
                        state.tmp.date_object = false;
                    }
                };
            }
            this.execs.push(func);

            // newoutput
            func = function (state, Item) {
                state.output.startTag("date", this);
            };
            this.execs.push(func);
        }

        if (!state.build.extension && (this.tokentype === CSL.END || this.tokentype === CSL.SINGLETON)) {
            // mergeoutput
            func = function (state, Item) {
                state.output.endTag();
                state.parallel.CloseVariable("date");
            };
            this.execs.push(func);
        }
        target.push(this);

        if (this.tokentype === CSL.END || this.tokentype === CSL.SINGLETON) {
            if (!state.build.extension) {
                CSL.Util.substituteEnd.call(this, state, target);
            }
        }
    }
};
