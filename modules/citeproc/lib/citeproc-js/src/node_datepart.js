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

CSL.Node["date-part"] = {
    build: function (state, target) {
        var func, pos, len, decor, first_date, value, value_end, real, have_collapsed, invoked, precondition, known_year, bc, ad, bc_end, ad_end, ready, curr, dcurr, number, num, formatter, item, i, ilen;
        if (!this.strings.form) {
            this.strings.form = "long";
        }
        // used in node_date, to send a list of rendering date parts
        // to node_key, for dates embedded in macros.
        state.build.date_parts.push(this.strings.name);
        //
        // Set delimiter here, if poss.
        //
        func = function (state, Item) {

            if (!state.tmp.date_object) {
                return;
            }

            first_date = true;
            value = "";
            value_end = "";
            state.tmp.donesies.push(this.strings.name);

            // Render literal only when year is included in date output
            if (state.tmp.date_object.literal && "year" === this.strings.name) {
                state.parallel.AppendToVariable(state.tmp.date_object.literal);
                state.output.append(state.tmp.date_object.literal, this);
            }

            if (state.tmp.date_object) {
                value = state.tmp.date_object[this.strings.name];
                value_end = state.tmp.date_object[(this.strings.name + "_end")];
            }
            if ("year" === this.strings.name && value === 0 && !state.tmp.suppress_decorations) {
                value = false;
            }
            real = !state.tmp.suppress_decorations;
            have_collapsed = state.tmp.have_collapsed;
            invoked = state[state.tmp.area].opt.collapse === "year-suffix" || state[state.tmp.area].opt.collapse === "year-suffix-ranged";
            precondition = state.opt["disambiguate-add-year-suffix"];
            if (real && precondition && invoked) {
                state.tmp.years_used.push(value);
                known_year = state.tmp.last_years_used.length >= state.tmp.years_used.length;
                if (known_year && have_collapsed) {
                    if (state.tmp.last_years_used[(state.tmp.years_used.length - 1)] === value) {
                        value = false;
                    }
                }
            }
            if ("undefined" !== typeof value) {
                bc = false;
                ad = false;
                bc_end = false;
                ad_end = false;
                if ("year" === this.strings.name) {
                    if (parseInt(value, 10) < 500 && parseInt(value, 10) > 0) {
                        ad = state.getTerm("ad");
                    }
                    if (parseInt(value, 10) < 0) {
                        bc = state.getTerm("bc");
                        value = (parseInt(value, 10) * -1);
                    }
                    if (value_end) {
                        if (parseInt(value_end, 10) < 500 && parseInt(value_end, 10) > 0) {
                            ad_end = state.getTerm("ad");
                        }
                        if (parseInt(value_end, 10) < 0) {
                            bc_end = state.getTerm("bc");
                            value_end = (parseInt(value_end, 10) * -1);
                        }
                    }
                }

                state.parallel.AppendToVariable(value);

                // For gendered locales
                var monthnameid = ""+state.tmp.date_object.month;
                while (monthnameid.length < 2) {
                    monthnameid = "0"+monthnameid;
                }
                monthnameid = "month-"+monthnameid;
                var gender = state.opt["noun-genders"][monthnameid];
                if (this.strings.form) {
                    value = CSL.Util.Dates[this.strings.name][this.strings.form](state, value, gender);
                    if ("month" === this.strings.name) {
                        // XXXXX Cut-and-paste code in multiple locations. This code block should be
                        // collected in a function.
                        // Tag: strip-periods-block
                        if (state.tmp.strip_periods) {
                            value = value.replace(/\./g, "");
                        } else {
                            for (i = 0, ilen = this.decorations.length; i < ilen; i += 1) {
                                if ("@strip-periods" === this.decorations[i][0] && "true" === this.decorations[i][1]) {
                                    value = value.replace(/\./g, "");
                                    break;
                                }
                            }
                        }
                        if (value_end) {
                            value_end = CSL.Util.Dates[this.strings.name][this.strings.form](state, value_end, gender);
                            // XXXXX Cut-and-paste code in multiple locations. This code block should be
                            // collected in a function.
                            // Tag: strip-periods-block
                            if (state.tmp.strip_periods) {
                                value_end = value_end.replace(/\./g, "");
                            } else {
                                for (i = 0, ilen = this.decorations.length; i < ilen; i += 1) {
                                    if ("@strip-periods" === this.decorations[i][0] && "true" === this.decorations[i][1]) {
                                        value_end = value_end.replace(/\./g, "");
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                state.output.openLevel("empty");
                if (state.tmp.date_collapse_at.length) {
                    //state.output.startTag(this.strings.name,this);
                    ready = true;
                    len = state.tmp.date_collapse_at.length;
                    for (pos = 0; pos < len; pos += 1) {
                        item = state.tmp.date_collapse_at[pos];
                        if (state.tmp.donesies.indexOf(item) === -1) {
                            ready = false;
                            break;
                        }
                    }
                    if (ready) {
                        if ("" + value_end !== "0") {
                            if (state.dateput.queue.length === 0) {
                                first_date = true;
                            }

                            // OK! So if the actual data has no month, day or season,
                            // and we reach this block, then we can combine the dates
                            // to a string, run minimial-two, and output the trailing
                            // year right here. No impact on other functionality.
                            
                            if (state.opt["page-range-format"] === "minimal-two"
                                && !state.tmp.date_object.day
                                && !state.tmp.date_object.month
                                && !state.tmp.date_object.season
                                && this.strings.name === "year"
                                && value && value_end) {
                                
                                // second argument adjusts collapse as required for years
                                // See OSCOLA section 1.3.2
                                value_end = state.fun.page_mangler(value + "-" + value_end, true);
                                var range_delimiter = state.getTerm("range-delimiter");
                                value_end = value_end.slice(value_end.indexOf(range_delimiter) + 1);
                            }
                            state.dateput.append(value_end, this);
                            if (first_date) {
                                state.dateput.current.value()[0].strings.prefix = "";
                            }
                        }
                        state.output.append(value, this);
                        curr = state.output.current.value();
                        curr.blobs[(curr.blobs.length - 1)].strings.suffix = "";
                        state.output.append(this.strings["range-delimiter"], "empty");
                        dcurr = state.dateput.current.value();
                        // if (dcurr.length){
                        // print("prefix: "+dcurr[0].blobs[0].strings.prefix);
                        // }
                        curr.blobs = curr.blobs.concat(dcurr);
                        state.dateput.string(state, state.dateput.queue);
                        state.tmp.date_collapse_at = [];
                    } else {
                        state.output.append(value, this);
                        // print("collapse_at: "+state.tmp.date_collapse_at);
                        if (state.tmp.date_collapse_at.indexOf(this.strings.name) > -1) {
                            //
                            // Use ghost dateput queue
                            //
                            if ("" + value_end !== "0") {
                                //
                                // XXXXX: It's a workaround.  It's ugly.
                                // There's another one above.
                                //
                                if (state.dateput.queue.length === 0) {
                                    first_date = true;
                                }
                                state.dateput.openLevel("empty");
                                state.dateput.append(value_end, this);
                                if (first_date) {
                                    state.dateput.current.value().blobs[0].strings.prefix = "";
                                }
                                if (bc) {
                                    state.dateput.append(bc);
                                }
                                if (ad) {
                                    state.dateput.append(ad);
                                }
                                state.dateput.closeLevel();
                            }
                        }
                    }
                } else {
                    state.output.append(value, this);
                }

                if (bc) {
                    state.output.append(bc);
                }
                if (ad) {
                    state.output.append(ad);
                }
                state.output.closeLevel();
                //state.output.endTag();
            } else if ("month" === this.strings.name) {
                // XXX The simpler solution here will be to
                // directly install season and season_end on
                // month, with a value of 13, 14, 15, 16, or
                // (to allow correct ranging with Down Under
                // dates) 17 or 18.  That will allow ranging
                // to take place in the normal way.  With this
                // "approach", it doesn't.
                //
                // No value for this target variable
                //
                if (state.tmp.date_object.season) {
                    value = "" + state.tmp.date_object.season;
                    if (value && value.match(/^[1-4]$/)) {
                        state.tmp.group_context.replace([false, false, true]);
                        state.output.append(state.getTerm(("season-0" + value)), this);
                    } else if (value) {
                        state.output.append(value, this);
                    }
                }
            }
            state.tmp.value = [];
            if ((value || state.tmp.have_collapsed) && !state.opt.has_year_suffix && "year" === this.strings.name && !state.tmp.just_looking) {
                if (state.registry.registry[Item.id] && state.registry.registry[Item.id].disambig.year_suffix !== false && !state.tmp.has_done_year_suffix) {
                    state.tmp.has_done_year_suffix = true;
                    num = parseInt(state.registry.registry[Item.id].disambig.year_suffix, 10);
                    number = new CSL.NumericBlob(num, this);
                    this.successor_prefix = state[state.build.area].opt.layout_delimiter;
                    this.splice_prefix = state[state.build.area].opt.layout_delimiter;
                    formatter = new CSL.Util.Suffixator(CSL.SUFFIX_CHARS);
                    number.setFormatter(formatter);
                    if (state[state.tmp.area].opt.collapse === "year-suffix-ranged") {
                        number.range_prefix = state.getTerm("range-delimiter");
                    }
                    if (state[state.tmp.area].opt["year-suffix-delimiter"]) {
                        number.successor_prefix = state[state.tmp.area].opt["year-suffix-delimiter"];
                    } else {
                        number.successor_prefix = state[state.tmp.area].opt.layout_delimiter;
                    }
                    number.UGLY_DELIMITER_SUPPRESS_HACK = true;
                    state.output.append(number, "literal");
                }
            }

        };
        this.execs.push(func);
        if ("undefined" === typeof this.strings["range-delimiter"]) {
            this.strings["range-delimiter"] = "\u2013";
        }
        target.push(this);
    }
};


