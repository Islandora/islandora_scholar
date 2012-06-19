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

CSL.Engine.prototype.makeBibliography = function (bibsection) {
    var debug, ret, params, maxoffset, item, len, pos, tok, tokk, tokkk, entry_ids, entry_strings, bibliography_errors;
    debug = false;
    // API change: added in version 1.0.51
    if (!this.bibliography.tokens.length) {
        return false;
    }
    if ("string" === typeof bibsection) {
        this.opt.citation_number_slug = bibsection;
        bibsection = false;
    }
    //SNIP-START
    if (debug) {
        len = this.bibliography.tokens.length;
        for (pos = 0; pos < len; pos += 1) {
            tok = this.bibliography.tokens[pos];
            CSL.debug("bibtok: " + tok.name);
        }
        CSL.debug("---");
        len = this.citation.tokens.length;
        for (pos = 0; pos < len; pos += 1) {
            tokk = this.citation.tokens[pos];
            CSL.debug("cittok: " + tok.name);
        }
        CSL.debug("---");
        len = this.bibliography_sort.tokens.length;
        for (pos = 0; pos < len; pos += 1) {
            tokkk = this.bibliography_sort.tokens[pos];
            CSL.debug("bibsorttok: " + tok.name);
        }
    }
    //SNIP-END
    ret = CSL.getBibliographyEntries.call(this, bibsection);
    entry_ids = ret[0];
    entry_strings = ret[1];
    params = {
        "maxoffset": 0,
        "entryspacing": this.bibliography.opt["entry-spacing"],
        "linespacing": this.bibliography.opt["line-spacing"],
        "second-field-align": false,
        "entry_ids": entry_ids,
        "bibliography_errors": this.tmp.bibliography_errors.slice()
    };
    if (this.bibliography.opt["second-field-align"]) {
        params["second-field-align"] = this.bibliography.opt["second-field-align"];
    }
    maxoffset = 0;
    len = this.registry.reflist.length;
    for (pos = 0; pos < len; pos += 1) {
        item = this.registry.reflist[pos];
        if (item.offset > params.maxoffset) {
            params.maxoffset = item.offset;
        }
    }
    if (this.bibliography.opt.hangingindent) {
        params.hangingindent = this.bibliography.opt.hangingindent;
    }
    params.bibstart = this.fun.decorate.bibstart;
    params.bibend = this.fun.decorate.bibend;

    this.opt.citation_number_slug = false;
    return [params, entry_strings];
};

/*
 * Compose individual cites into a single string.
 */
CSL.getBibliographyEntries = function (bibsection) {
    var ret, input, include, anymatch, allmatch, bib_entry, res, len, pos, item, llen, ppos, spec, lllen, pppos, bib_layout, topblobs, all_item_ids, entry_item_ids, debug, collapse_parallel, i, ilen, siblings, skips, sortedItems, eyetem, chr, entry_item_data;
    ret = [];
    entry_item_data = [];
    this.tmp.area = "bibliography";
    this.tmp.last_rendered_name = false;
    this.tmp.bibliography_errors = [];
    this.tmp.bibliography_pos = 0;

    // cs:generate needs to be applied here, apparently.
    // spoof IDs for inserts, and be sure to amend all_item_ids as well.
    // First, make a note of the original list of IDs
    
    // registry.generate has:
    //   bundles
    //   rules
    // rules are used in registry processing to identify items
    // for which a bundle should be created. A bundle is a set
    // of rules with an item ID. The actual item is fetched
    // on demand during bundle processing when the bibliography
    // is generated (i.e. here).

    var originalIDs = this.registry.getSortedIds();
    var newIDs = [];
    this.registry.generate.items = {};

    // generate has two forks: origIDs and genIDs
    for (var id  in this.registry.generate.origIDs) {
        // get the bundle
        var rule = this.registry.generate.origIDs[id];
        // clone the attached item. clone is shallow, just the top-level keys.
        item = this.retrieveItem(id);
        var clonedItem = {};
        for (var key in item) {
            clonedItem[key] = item[key];
        }
        // remap the item type
        clonedItem.type = rule.to;
        // remove "required" field(s)
        for (i = 0, ilen = rule.triggers.length; i < ilen; i += 1) {
            if (clonedItem[rule.triggers[i]]) {
                delete clonedItem[rule.triggers[i]];
            }
        }
        // amend itemID (to the form set on genIDs fork)
        var newID = clonedItem.id + ":gen";
        clonedItem.id = newID;
        // add to generated items, will be picked up by retrieve function
        this.registry.generate.items[clonedItem.id] = clonedItem;
        // add new ID to list
        newIDs.push(newID);
    }
    if (newIDs.length) {
        this.updateItems(originalIDs.concat(newIDs));
    }
    // retrieveItems will pick up the generated items
    input = this.retrieveItems(this.registry.getSortedIds());

    this.tmp.disambig_override = true;
    function eval_string(a, b) {
        if (a === b) {
            return true;
        }
        return false;
    }
    function eval_list(a, lst) {
        lllen = lst.length;
        for (pppos = 0; pppos < lllen; pppos += 1) {
            if (eval_string(a, lst[pppos])) {
                return true;
            }
        }
        return false;
    }
    function eval_spec(a, b) {
        if ((a === "none" || !a) && !b) {
            return true;
        }
        if ("string" === typeof b) {
            return eval_string(a, b);
        } else if (!b) {
            return false;
        } else {
            return eval_list(a, b);
        }
    }

    skips = {};
    all_item_ids = [];
    len = input.length;
    for (pos = 0; pos < len; pos += 1) {
        item = input[pos];
        if (skips[item.id]) {
            continue;
        }
        if (bibsection) {
            include = true;
            if (bibsection.include) {
                //
                // Opt-in: these are OR-ed.
                //
                include = false;
                llen = bibsection.include.length;
                for (ppos = 0; ppos < llen; ppos += 1) {
                    spec = bibsection.include[ppos];
                    if (eval_spec(spec.value, item[spec.field])) {
                        include = true;
                        break;
                    }
                }
            } else if (bibsection.exclude) {
                //
                // Opt-out: these are also OR-ed.
                //
                anymatch = false;
                llen = bibsection.exclude.length;
                for (ppos = 0; ppos < llen; ppos += 1) {
                    spec = bibsection.exclude[ppos];
                    if (eval_spec(spec.value, item[spec.field])) {
                        anymatch = true;
                        break;
                    }
                }
                if (anymatch) {
                    include = false;
                }
            } else if (bibsection.select) {
                //
                // Multiple condition opt-in: these are AND-ed.
                //
                include = false;
                allmatch = true;
                llen = bibsection.select.length;
                for (ppos = 0; ppos < llen; ppos += 1) {
                    spec = bibsection.select[ppos];
                    if (!eval_spec(spec.value, item[spec.field])) {
                        allmatch = false;
                    }
                }
                if (allmatch) {
                    include = true;
                }
            }
            if (bibsection.quash) {
                //
                // Stop criteria: These are AND-ed.
                //
                allmatch = true;
                llen = bibsection.quash.length;
                for (ppos = 0; ppos < llen; ppos += 1) {
                    spec = bibsection.quash[ppos];
                    if (!eval_spec(spec.value, item[spec.field])) {
                        allmatch = false;
                    }
                }
                if (allmatch) {
                    include = false;
                }
            }
            if (!include) {
                continue;
            }
        }
        //SNIP-START
        if (debug) {
            CSL.debug("BIB: " + item.id);
        }
        //SNIP-END
        bib_entry = new CSL.Token("group", CSL.START);
        bib_entry.decorations = [["@bibliography", "entry"]].concat(this[this.build.area].opt.layout_decorations);

        this.output.startTag("bib_entry", bib_entry);
        this.output.current.value().item_id = item.id;

        // The needs fixing.  Parallel cite should be generated
        // by arrival of either a master or a sibling, with the
        // same result.

        sortedItems = [[{id: "" + item.id}, item]];
        entry_item_ids = [];
        if (this.registry.registry[item.id].master) {
            collapse_parallel = true;
            this.parallel.StartCitation(sortedItems);
            this.output.queue[0].strings.delimiter = ", ";
            this.tmp.term_predecessor = false;
            entry_item_ids.push("" + CSL.getCite.call(this, item));
            skips[item.id] = true;
            siblings = this.registry.registry[item.id].siblings;
            for (ppos = 0, llen = siblings.length; ppos < llen; ppos += 1) {
                i = this.registry.registry[item.id].siblings[ppos];
                eyetem = this.retrieveItem(i);
                entry_item_ids.push("" + CSL.getCite.call(this, eyetem));
                skips[eyetem.id] = true;
            }
            this.parallel.ComposeSet();
            this.parallel.PruneOutputQueue();
        } else if (!this.registry.registry[item.id].siblings) {
            this.tmp.term_predecessor = false;
            entry_item_ids.push("" + CSL.getCite.call(this, item));
            //skips[item.id] = true;
        }
        // For RDF support
        entry_item_data.push("");

        this.tmp.bibliography_pos += 1;

        all_item_ids.push(entry_item_ids);
        //
        // XXX: loop to render parallels goes here
        // XXX: just have to mark them somehow ...
        //
        this.output.endTag("bib_entry");
        //
        // place layout prefix on first blob of each cite, and suffix
        // on the last non-empty blob of each cite.  there be dragons
        // here.
        //
        if (this.output.queue[0].blobs.length && this.output.queue[0].blobs[0].blobs.length) {
            // The output queue stuff needs cleaning up.  the result of
            // output.current.value() is sometimes a blob, sometimes its list
            // of blobs.  this inconsistency is a source of confusion, and
            // should be cleaned up across the code base in the first
            // instance, before making any other changes to output code.
            if (collapse_parallel || !this.output.queue[0].blobs[0].blobs[0].strings) {
                topblobs = this.output.queue[0].blobs;
                collapse_parallel = false;
            } else {
                topblobs = this.output.queue[0].blobs[0].blobs;
            }
            llen = topblobs.length - 1;
            for (ppos = llen; ppos > -1; ppos += -1) {
                if (topblobs[ppos].blobs && topblobs[ppos].blobs.length !== 0) {
                    // Fix up duplicate terminal punctuation, reported by Carles Pina 2010-07-15
                    chr = this.bibliography.opt.layout_suffix.slice(0, 1);
                    if (chr && topblobs[ppos].strings.suffix.slice(-1) === chr) {
                        topblobs[ppos].strings.suffix = topblobs[ppos].strings.suffix.slice(0, -1);
                    }
                    topblobs[ppos].strings.suffix += this.bibliography.opt.layout_suffix;
                    break;
                }
            }
            topblobs[0].strings.prefix = this.bibliography.opt.layout_prefix + topblobs[0].strings.prefix;
        }
        CSL.Output.Queue.purgeEmptyBlobs(this.output.queue);
        CSL.Output.Queue.adjustPunctuation(this, this.output.queue);
        res = this.output.string(this, this.output.queue)[0];
        if (!res) {
            res = "\n[CSL STYLE ERROR: reference with no printed form.]\n";
        }
        ret.push(res);
    }

    // reset list if spoofed entries were included
    if (newIDs.length) {
        this.updateItems(originalIDs);
    }

    this.tmp.disambig_override = false;
    return [all_item_ids, ret];
};
