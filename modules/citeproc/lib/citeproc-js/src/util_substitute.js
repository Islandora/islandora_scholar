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

CSL.Util.substituteStart = function (state, target) {
    var element_trace, display, bib_first, func, choose_start, if_start, nodetypes;
    func = function (state, Item) {
        for (var i = 0, ilen = this.decorations.length; i < ilen; i += 1) {
            if ("@strip-periods" === this.decorations[i][0] && "true" === this.decorations[i][1]) {
                state.tmp.strip_periods += 1;
                break;
            }
        }
    };
    this.execs.push(func);
    //
    // Contains body code for both substitute and first-field/remaining-fields
    // formatting.
    //
    nodetypes = ["number", "date", "names"];
    if (("text" === this.name && !this.postponed_macro) || nodetypes.indexOf(this.name) > -1) {
        element_trace = function (state, Item, item) {
            if (state.tmp.element_trace.value() === "author" || "names" === this.name) {
                if (item && item["author-only"]) {
                    state.tmp.element_trace.push("do-not-suppress-me");
                } else if (item && item["suppress-author"]) {
                    // This is better handled by namesOutput()
                    //state.tmp.element_trace.push("suppress-me");
                }
            } else {
                if (item && item["author-only"]) {
                    state.tmp.element_trace.push("suppress-me");
                } else if (item && item["suppress-author"]) {
                    state.tmp.element_trace.push("do-not-suppress-me");
                }
            }
        };
        this.execs.push(element_trace);
    }
    display = this.strings.cls;
    this.strings.cls = false;
    if (state.build.render_nesting_level === 0) {
        //
        // The markup formerly known as @bibliography/first
        //
        // Separate second-field-align from the generic display logic.
        // There will be some code replication, but not in the
        // assembled style.
        //
        if (state.build.area === "bibliography" && state.bibliography.opt["second-field-align"]) {
            bib_first = new CSL.Token("group", CSL.START);
            bib_first.decorations = [["@display", "left-margin"]];
            func = function (state, Item) {
                if (!state.tmp.render_seen) {
                    bib_first.strings.first_blob = Item.id;
                    state.output.startTag("bib_first", bib_first);
                }
            };
            bib_first.execs.push(func);
            target.push(bib_first);
        } else if (CSL.DISPLAY_CLASSES.indexOf(display) > -1) {
            bib_first = new CSL.Token("group", CSL.START);
            bib_first.decorations = [["@display", display]];
            func = function (state, Item) {
                bib_first.strings.first_blob = Item.id;
                state.output.startTag("bib_first", bib_first);
            };
            bib_first.execs.push(func);
            target.push(bib_first);
        }
        state.build.cls = display;
    }
    state.build.render_nesting_level += 1;
    // Should this be render_nesting_level, with the increment
    // below? ... ?
    if (state.build.substitute_level.value() === 1) {
        //
        // All top-level elements in a substitute environment get
        // wrapped in conditionals.  The substitute_level variable
        // is a stack, because spanned names elements (with their
        // own substitute environments) can be nested inside
        // a substitute environment.
        //
        // (okay, we use conditionals a lot more than that.
        // we slot them in for author-only as well...)
        choose_start = new CSL.Token("choose", CSL.START);
        CSL.Node.choose.build.call(choose_start, state, target);
        if_start = new CSL.Token("if", CSL.START);
        //
        // Set a test of the shadow if token to skip this
        // macro if we have acquired a name value.

        // check for variable
        func = function (state, Item) {
            if (state.tmp.can_substitute.value()) {
                return true;
            }
            return false;
        };
        if_start.tests.push(func);
        //
        // this is cut-and-paste of the "any" evaluator
        // function, from Attributes.  These functions
        // should be defined in a namespace for reuse.
        // Sometime.
        if_start.evaluator = state.fun.match.any;
        target.push(if_start);
    }
};


CSL.Util.substituteEnd = function (state, target) {
    var func, bib_first_end, bib_other, if_end, choose_end, toplevel, hasval, author_substitute, str;
    func = function (state, Item) {
        for (var i = 0, ilen = this.decorations.length; i < ilen; i += 1) {
            if ("@strip-periods" === this.decorations[i][0] && "true" === this.decorations[i][1]) {
                state.tmp.strip_periods += -1;
                break;
            }
        }
    };
    this.execs.push(func);

    state.build.render_nesting_level += -1;
    if (state.build.render_nesting_level === 0) {
        if (state.build.cls) {
            func = function (state, Item) {
                state.output.endTag("bib_first");
            };
            this.execs.push(func);
            state.build.cls = false;
        } else if (state.build.area === "bibliography" && state.bibliography.opt["second-field-align"]) {
            bib_first_end = new CSL.Token("group", CSL.END);
            // first func end
            func = function (state, Item) {
                if (!state.tmp.render_seen) {
                    state.output.endTag(); // closes bib_first
                }
            };
            bib_first_end.execs.push(func);
            target.push(bib_first_end);
            bib_other = new CSL.Token("group", CSL.START);
            bib_other.decorations = [["@display", "right-inline"]];
            func = function (state, Item) {
                if (!state.tmp.render_seen) {
                    state.tmp.render_seen = true;
                    state.output.startTag("bib_other", bib_other);
                }
            };
            bib_other.execs.push(func);
            target.push(bib_other);
        }
    }
    if (state.build.substitute_level.value() === 1) {
        if_end = new CSL.Token("if", CSL.END);
        target.push(if_end);
        choose_end = new CSL.Token("choose", CSL.END);
        CSL.Node.choose.build.call(choose_end, state, target);
    }

    toplevel = "names" === this.name && state.build.substitute_level.value() === 0;
    hasval = "string" === typeof state[state.build.area].opt["subsequent-author-substitute"];
    var subrule = state[state.build.area].opt["subsequent-author-substitute-rule"];
    if (toplevel && hasval) {
        author_substitute = new CSL.Token("text", CSL.SINGLETON);
        func = function (state, Item) {
            var i, ilen;
            var text_esc = CSL.getSafeEscape(state.opt.mode, state.tmp.area);
            var printing = !state.tmp.suppress_decorations;
            if (printing && state.tmp.area === "bibliography") {
                if (!state.tmp.rendered_name) {
                    if ("partial-each" === subrule || "partial-first" === subrule) {
                        state.tmp.rendered_name = [];
                        var dosub = true;
                        for (i = 0, ilen = state.tmp.name_node.children.length; i < ilen; i += 1) {
                            var name = state.output.string(state, state.tmp.name_node.children[i].blobs, false);
                            if (dosub
                                && state.tmp.last_rendered_name && state.tmp.last_rendered_name.length > i - 1
                                && state.tmp.last_rendered_name[i] === name) {
                                
                                str = new CSL.Blob(text_esc(state[state.tmp.area].opt["subsequent-author-substitute"]));
                                state.tmp.name_node.children[i].blobs = [str];
                                if ("partial-first" === subrule) {
                                    dosub = false;
                                }
                            } else {
                                dosub = false;
                            }
                            state.tmp.rendered_name.push(name);
                        }
                        // might want to slice this?
                        state.tmp.last_rendered_name = state.tmp.rendered_name;
                    } else if ("complete-each" === subrule) {
                        state.tmp.rendered_name = state.output.string(state, state.tmp.name_node.top.blobs, false);
                        if (state.tmp.rendered_name) {
                            if (state.tmp.rendered_name === state.tmp.last_rendered_name) {
                                for (i = 0, ilen = state.tmp.name_node.children.length; i < ilen; i += 1) {
                                    str = new CSL.Blob(text_esc(state[state.tmp.area].opt["subsequent-author-substitute"]));
                                    state.tmp.name_node.children[i].blobs = [str];
                                }
                            }
                            state.tmp.last_rendered_name = state.tmp.rendered_name;
                        }
                    } else {
                        state.tmp.rendered_name = state.output.string(state, state.tmp.name_node.top.blobs, false);
                        if (state.tmp.rendered_name) {
                            //CSL.debug("TRY! "+state.tmp.rendered_name);
                            if (state.tmp.rendered_name === state.tmp.last_rendered_name) {
                                str = new CSL.Blob(text_esc(state[state.tmp.area].opt["subsequent-author-substitute"]));
                                state.tmp.name_node.top.blobs = [str];
                            }
                            state.tmp.last_rendered_name = state.tmp.rendered_name;
                        }
                    }
                }
            }
        };
        author_substitute.execs.push(func);
        target.push(author_substitute);
    }

    if (("text" === this.name && !this.postponed_macro) || ["number", "date", "names"].indexOf(this.name) > -1) {
        // element trace
        func = function (state, Item) {
            state.tmp.element_trace.pop();
        };
        this.execs.push(func);
    }
};
