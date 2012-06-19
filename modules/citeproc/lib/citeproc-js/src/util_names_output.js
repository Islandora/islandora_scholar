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

CSL.NameOutput = function(state, Item, item, variables) {
    this.debug = false;
    //SNIP-START
    if (this.debug) {
        print("(1)");
    }
    //SNIP-END
    this.state = state;
    this.Item = Item;
    this.item = item;
    this.nameset_base = 0;
    this._first_creator_variable = false;
    this._please_chop = false;
};

CSL.NameOutput.prototype.init = function (names) {
    if (this.nameset_offset) {
        this.nameset_base = this.nameset_base + this.nameset_offset;
    }
    this.nameset_offset = 0;
    this.names = names;
    this.variables = names.variables;
    this.state.tmp.value = [];
    for (var i = 0, ilen = this.variables.length; i < ilen; i += 1) {
        if (this.Item[this.variables[i]] && this.Item[this.variables[i]].length) {
            this.state.tmp.value = this.state.tmp.value.concat(this.Item[this.variables[i]]);
        }
    }
    this["et-al"] = undefined;
    this["with"] = undefined;
    this.name = undefined;
    // long, long-with-short, short
    this.institutionpart = {};
    // family, given
    //this.namepart = {};
    // before, after
    //this.label = {};
    
    this.state.tmp.group_context.value()[1] = true;

    if (!this.state.tmp.value.length) {
        return;
    }

    // Set to true if something happens
    this.state.tmp.group_context.value()[2] = false;
};


CSL.NameOutput.prototype.reinit = function (names) {
    // Wrong control var, surely
    //if (true) {
    if (this.state.tmp.can_substitute.value()) {
    //if (!this.state.tmp.group_context.value()[2]) {
        this.nameset_offset = 0;
        // What-all should be carried across from the subsidiary
        // names node, and on what conditions? For each attribute,
        // and decoration, is it an override, or is it additive?
        this.variables = names.variables;
        // Set to true if something happens
        //this.state.tmp.group_context.value()[2] = false;
        //if (this.state.tmp.group_context.value()[2]) {
        //    this.state.tmp.group_context.value()[2] = true;
        //}
        var oldval = this.state.tmp.value.slice();
        this.state.tmp.value = [];
        for (var i = 0, ilen = this.variables.length; i < ilen; i += 1) {
            if (this.Item[this.variables[i]] && this.Item[this.variables[i]].length) {
                this.state.tmp.value = this.state.tmp.value.concat(this.Item[this.variables[i]]);
            }
        }
        if (this.state.tmp.value.length) {
            this.state.tmp.can_substitute.replace(false, CSL.LITERAL);
        }
        this.state.tmp.value = oldval;
    }
};

CSL.NameOutput.prototype.outputNames = function () {
    var i, ilen;
    var variables = this.variables;

    if (this.institution.and) {
        if (!this.institution.and.single.blobs && !this.institution.and.single.blobs.length) {
            this.institution.and.single.blobs = this.name.and.single.blobs;
        }
        if (!this.institution.and.single.blobs && !this.institution.and.multiple.blobs.length) {
            this.institution.and.multiple.blobs = this.name.and.multiple.blobs;
        }
    }

    this.variable_offset = {};
    if (this.family) {
        this.family_decor = CSL.Util.cloneToken(this.family);
        this.family_decor.strings.prefix = "";
        this.family_decor.strings.suffix = "";
        // Sets text-case value (text-case="title" is suppressed for items
        // non-English with non-English value in Item.language)
        for (i = 0, ilen = this.family.execs.length; i < ilen; i += 1) {
            this.family.execs[i].call(this.family_decor, this.state, this.Item);
        }
    } else {
        this.family_decor = false;
    }

    if (this.given) {
        this.given_decor = CSL.Util.cloneToken(this.given);
        this.given_decor.strings.prefix = "";
        this.given_decor.strings.suffix = "";
        // Sets text-case value (text-case="title" is suppressed for items
        // non-English with non-English value in Item.language)
        for (i = 0, ilen = this.given.execs.length; i < ilen; i += 1) {
            this.given.execs[i].call(this.given_decor, this.state, this.Item);
        }
    } else {
        this.given_decor = false;
    }

    //SNIP-START
    if (this.debug) {
        print("(2)");
    }
    //SNIP-END
    // util_names_etalconfig.js
    this.getEtAlConfig();
    //SNIP-START
    if (this.debug) {
        print("(3)");
    }
    //SNIP-END
    // util_names_divide.js
    this.divideAndTransliterateNames();
    //SNIP-START
    if (this.debug) {
        print("(4)");
    }
    //SNIP-END
    // util_names_truncate.js
    this.truncatePersonalNameLists();
    //SNIP-START
    if (this.debug) {
        print("(5)");
    }
    //SNIP-END
    // util_names_constraints.js
    this.constrainNames();
    //SNIP-START
    if (this.debug) {
        print("(6)");
    }
    //SNIP-END
    // form="count"
    if (this.name.strings.form === "count") {
        if (this.state.tmp.extension || this.names_count != 0) {
            this.state.output.append(this.names_count, "empty");
            this.state.tmp.group_context.value()[2] = true;
        }
        //else {
        //    this.state.tmp.group_context.value()[1] = true;
        //    
        //    this.state.tmp.group_context.value()[2] = false;
        //}
        return;
    }

    //SNIP-START
    if (this.debug) {
        print("(7)");
    }
    //SNIP-END
    // util_names_disambig.js
    this.disambigNames();
    //SNIP-START
    if (this.debug) {
        print("(8)");
    }
    //SNIP-END
    this.setEtAlParameters();
    //SNIP-START
    if (this.debug) {
        print("(9)");
    }
    //SNIP-END
    this.setCommonTerm();
    //SNIP-START
    if (this.debug) {
        print("(10)");
    }
    //SNIP-END
    this.state.tmp.name_node = {};
    this.state.tmp.name_node.children = [];
    this.renderAllNames();
    //SNIP-START
    if (this.debug) {
        print("(11)");
    }
    //SNIP-END
    var blob_list = [];
    for (i = 0, ilen = variables.length; i < ilen; i += 1) {
        var v = variables[i];
        var institution_sets = [];
        var institutions = false;
        //SNIP-START
        if (this.debug) {
            print("(11a)");
        }
        //SNIP-END
        for (var j = 0, jlen = this.institutions[v].length; j < jlen; j += 1) {
            institution_sets.push(this.joinPersonsAndInstitutions([this.persons[v][j], this.institutions[v][j]]));
        }
        //SNIP-START
        if (this.debug) {
            print("(11b)");
        }
        //SNIP-END
        if (this.institutions[v].length) {
            var pos = this.nameset_base + this.variable_offset[v];
            if (this.freeters[v].length) {
                pos += 1;
            }
            institutions = this.joinInstitutionSets(institution_sets, pos);
        }
        //SNIP-START
        if (this.debug) {
            print("(11c)");
        }
        //SNIP-END
        var varblob = this.joinFreetersAndInstitutionSets([this.freeters[v], institutions]);
        //SNIP-START
        if (this.debug) {
            print("(11d)");
        }
        //SNIP-END
        if (varblob) {
            // Apply labels, if any
            varblob = this._applyLabels(varblob, v);
            blob_list.push(varblob);
        }
        //SNIP-START
        if (this.debug) {
            print("(11e)");
        }
        //SNIP-END
        if (this.common_term) {
            break;
        }
    }
    //SNIP-START
    if (this.debug) {
        print("(12)");
    }
    //SNIP-END
    this.state.output.openLevel("empty");
    this.state.output.current.value().strings.delimiter = this.names.strings.delimiter;
    //SNIP-START
    if (this.debug) {
        print("(13)");
    }
    //SNIP-END
    for (i = 0, ilen = blob_list.length; i < ilen; i += 1) {
        // notSerious
        this.state.output.append(blob_list[i], "literal", true);
    }
    //SNIP-START
    if (this.debug) {
        print("(14)");
    }
    //SNIP-END
    this.state.output.closeLevel("empty");
    //SNIP-START
    if (this.debug) {
        print("(15)");
    }
    //SNIP-END
    var blob = this.state.output.pop();
    //SNIP-START
    if (this.debug) {
        print("(16)");
    }
    //SNIP-END
    this.state.output.append(blob, this.names);
    //SNIP-START
    if (this.debug) {
        print("(17)");
    }
    //SNIP-END
    // Also used in CSL.Util.substituteEnd (which could do with
    // some cleanup at this writing).
    //SNIP-START
    if (this.debug) {
        print("(18)");
    }
    //SNIP-END
    this.state.tmp.name_node.top = this.state.output.current.value();

    // Load and check for classic abbreviation, ONLY if the
    // current item has a nil type.
    //
    // If found, then (1) suppress title rendering, (2) replace the node
    // with the abbreviation output [and (3) do not run this._collapseAuthor() ?]
    var oldSuppressDecorations = this.state.tmp.suppress_decorations;
    this.state.tmp.suppress_decorations = true;
    var lastBlob = this.state.tmp.name_node.top.blobs.pop();
    var name_node_string = this.state.output.string(this.state, lastBlob.blobs, false);
    this.state.tmp.name_node.top.blobs.push(lastBlob);
    if (name_node_string) {
        this.state.tmp.name_node.string = name_node_string;
    }
    this.state.tmp.suppress_decorations = oldSuppressDecorations;
    // for hereinafter support
    if (this.state.tmp.name_node.string && !this.state.tmp.first_name_string) {
        this.state.tmp.first_name_string = this.state.tmp.name_node.string;
    }
    if ("classic" === this.Item.type) {
        var author_title = [];
        if (this.state.tmp.first_name_string) {
            author_title.push(this.state.tmp.first_name_string);
        }
        if (this.Item.title) {
            author_title.push(this.Item.title);
        }
        author_title = author_title.join(", ");
        if (author_title && this.state.sys.getAbbreviation) {
            this.state.transform.loadAbbreviation("default", "classic", author_title);
            if (this.state.transform.abbrevs["default"].classic[author_title]) {
                this.state.tmp.done_vars.push("title");
                this.state.output.append(this.state.transform.abbrevs["default"].classic[author_title], "empty", true);
                blob = this.state.output.pop();
				this.state.tmp.name_node.top.blobs.pop();
                this.state.tmp.name_node.top.blobs.push(blob);
            }
        }
    }

    if (this.Item.type === "personal_communication" || this.Item.type === "interview") {
        var author = "";
        author = this.state.tmp.name_node.string;
        if (author && this.state.sys.getAbbreviation && !(this.item && this.item["suppress-author"])) {
            this.state.transform.loadAbbreviation("default", "nickname", author);
            var myLocalName = this.state.transform.abbrevs["default"].nickname[author];
            if (myLocalName) {
                if (myLocalName === "{suppress}") {
                    this.state.tmp.name_node.top.blobs.pop();
                    this.state.tmp.group_context.value()[2] = false;
                } else {
                    this.state.output.append(myLocalName, "empty", true)
                    blob = this.state.output.pop();
                    this.state.tmp.name_node.top.blobs = [blob];
                }
            }
        }
    }
    // Let's try something clever here.
    this._collapseAuthor();

    // For name_SubstituteOnNamesSpanNamesSpanFail
    this.variables = [];
    //SNIP-START
    if (this.debug) {
        print("(19)");
    }
    //SNIP-END
};

CSL.NameOutput.prototype._applyLabels = function (blob, v) {
    var txt;
    if (!this.label) {
        return blob;
    }
    var plural = 0;
    var num = this.freeters_count[v] + this.institutions_count[v];
    if (num > 1) {
        plural = 1;
    } else {
        for (var i = 0, ilen = this.persons[v].length; i < ilen; i += 1) {
            num += this.persons_count[v][i];
        }
        if (num > 1) {
            plural = 1;
        }
    }
    // Some code duplication here, should be factored out.
    if (this.label.before) {
        if ("number" === typeof this.label.before.strings.plural) {
            plural = this.label.before.strings.plural;
        }
        txt = this._buildLabel(v, plural, "before");
        this.state.output.openLevel("empty");
        this.state.output.append(txt, this.label.before, true);
        this.state.output.append(blob, "literal", true);
        this.state.output.closeLevel("empty");
        blob = this.state.output.pop();
    }
    if (this.label.after) {
        if ("number" === typeof this.label.after.strings.plural) {
            plural = this.label.after.strings.plural;
        }
        txt = this._buildLabel(v, plural, "after");
        this.state.output.openLevel("empty");
        this.state.output.append(blob, "literal", true);
        this.state.output.append(txt, this.label.after, true);
        this.state.output.closeLevel("empty");
        blob = this.state.output.pop();
    }
    return blob;
};

CSL.NameOutput.prototype._buildLabel = function (term, plural, position) {
    if (this.common_term) {
        term = this.common_term;
    }

    var ret = false;
    var node = this.label[position];
    if (node) {
        ret = CSL.castLabel(this.state, node, term, plural);
    }
    return ret;
};


CSL.NameOutput.prototype._collapseAuthor = function () {
    var myqueue, mystr, oldchars;
    // collapse can be undefined, an array of length zero, and probably
    // other things ... ugh.
    if (this.nameset_base === 0 && this.Item[this.variables[0]] && !this._first_creator_variable) {
        this._first_creator_variable = this.variables[0];
    }
    if ((this.item && this.item["suppress-author"] && this._first_creator_variable == this.variables[0])
        || (this.state[this.state.tmp.area].opt.collapse 
            && this.state[this.state.tmp.area].opt.collapse.length)) {

        if (this.state.tmp.authorstring_request) {
            // Avoid running this on every call to getAmbiguousCite()?
            mystr = "";
            myqueue = this.state.tmp.name_node.top.blobs.slice(-1)[0].blobs;
            oldchars = this.state.tmp.offset_characters;
            if (myqueue) {
                mystr = this.state.output.string(this.state, myqueue, false);
            }
            // Avoid side-effects on character counting: we're only interested
            // in the final rendering.
            this.state.tmp.offset_characters = oldchars;
            this.state.registry.authorstrings[this.Item.id] = mystr;
        } else if (!this.state.tmp.just_looking
            && !this.state.tmp.suppress_decorations) {

            // XX1 print("RENDER: "+this.Item.id);
            mystr = "";
            myqueue = this.state.tmp.name_node.top.blobs.slice(-1)[0].blobs;
            oldchars = this.state.tmp.offset_characters;
            if (myqueue) {
                mystr = this.state.output.string(this.state, myqueue, false);
            }
            if (mystr === this.state.tmp.last_primary_names_string) {
            
                // XX1 print("    CUT!");
                this.state.tmp.name_node.top.blobs.pop();
                this.state.tmp.name_node.children = [];
                // If popped, avoid side-effects on character counting: we're only interested
                // in things that actually render.
                this.state.tmp.offset_characters = oldchars;
            } else {
                // XX1 print("remembering: "+mystr);
                this.state.tmp.last_primary_names_string = mystr;

                // XXXXX A little more precision would be nice.
                // This will clobber variable="author editor" as well as variable="author".
                if (this.variables.indexOf(this._first_creator_variable) > -1 && this.item && this.item["suppress-author"] && this.Item.type !== "legal_case") {
                    this.state.tmp.name_node.top.blobs.pop();
                    this.state.tmp.name_node.children = [];
                    // If popped, avoid side-effects on character counting: we're only interested
                    // in things that actually render.
                    this.state.tmp.offset_characters = oldchars;

                    // A wild guess, but will usually be correct
                    this.state.tmp.term_predecessor = false;
                }
                // Arcane and probably unnecessarily complicated
                this.state.tmp.have_collapsed = false;
            }
        }
    }
};

/*
CSL.NameOutput.prototype.suppressNames = function() {
    suppress_condition = suppress_min && display_names.length >= suppress_min;
    if (suppress_condition) {
        continue;
    }
}
*/
