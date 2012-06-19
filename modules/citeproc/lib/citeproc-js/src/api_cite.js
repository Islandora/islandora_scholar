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

CSL.Engine.prototype.previewCitationCluster = function (citation, citationsPre, citationsPost, newMode) {
    // Generate output for a hypothetical citation at the current position,
    // Leave the registry in the same state in which it was found.
    //print("################### previewCitationCluster() #################");
    var oldMode = this.opt.mode;
    this.setOutputFormat(newMode);

    var ret = this.processCitationCluster(citation, citationsPre, citationsPost, CSL.PREVIEW);

    this.setOutputFormat(oldMode);
    return ret[1];
};

CSL.Engine.prototype.appendCitationCluster = function (citation) {
    var citationsPre = [];
    var len = this.registry.citationreg.citationByIndex.length;
    for (var pos = 0; pos < len; pos += 1) {
        var c = this.registry.citationreg.citationByIndex[pos];
        citationsPre.push(["" + c.citationID, c.properties.noteIndex]);
    }
    // Drop the data segment to return a list of pos/string pairs.
    return this.processCitationCluster(citation, citationsPre, [])[1];
};


CSL.Engine.prototype.processCitationCluster = function (citation, citationsPre, citationsPost, flag) {
    var c, i, ilen, j, jlen, k, klen, n, nlen, key, Item, item, noteCitations, textCitations;
    this.debug = false;
    //print("################### processCitationCluster() #################");
    //SNIP-START
    // this.dumpCslCitation(citation, flag);
    //SNIP-END
    this.tmp.citation_errors = [];
    var return_data = {"bibchange": false};
    this.registry.return_data = return_data;

    // make sure this citation has a unique ID, and register it in citationById.
    this.setCitationId(citation);

    var oldCitationList;
    var oldItemList;
    var oldAmbigs;
    if (flag === CSL.PREVIEW) {
        //SNIP-START
        if (this.debug) {
            CSL.debug("****** start state save *********");
        }
        //SNIP-END
        //
        // Simplify.

        // Take a slice of existing citations.
        oldCitationList = this.registry.citationreg.citationByIndex.slice();

        // Take a slice of current items, for later use with update.
        oldItemList = this.registry.reflist.slice();

        // Make a list of preview citation ref objects
        var newCitationList = citationsPre.concat([["" + citation.citationID, citation.properties.noteIndex]]).concat(citationsPost);

        // Make a full list of desired ids, for use in preview update,
        // and a hash list of same while we're at it.
        var newItemIds = {};
        var newItemIdsList = [];
        for (i = 0, ilen = newCitationList.length; i < ilen; i += 1) {
            c = this.registry.citationreg.citationById[newCitationList[i][0]];
            for (j = 0, jlen = c.citationItems.length; j < jlen; j += 1) {
                newItemIds[c.citationItems[j].id] = true;
                newItemIdsList.push("" + c.citationItems[j].id);
            }
        }

        // Clone and save off disambigs of items that will be lost.
        oldAmbigs = {};
        for (i = 0, ilen = oldItemList.length; i < ilen; i += 1) {
            if (!newItemIds[oldItemList[i].id]) {
                var oldAkey = this.registry.registry[oldItemList[i].id].ambig;
                var ids = this.registry.ambigcites[oldAkey];
                if (ids) {
                    for (j = 0, jlen = ids.length; j < jlen; j += 1) {
                        oldAmbigs[ids[j]] = CSL.cloneAmbigConfig(this.registry.registry[ids[j]].disambig);
                    }
                }
            }
        }

        // Update items.  This will produce the base name data and sort things.
        // Possibly unnecessary?
        //this.updateItems(this.registry.mylist.concat(tmpItems));

        //SNIP-START
        if (this.debug) {
            CSL.debug("****** end state save *********");
        }
        //SNIP-END
    }

    this.tmp.taintedItemIDs = {};
    this.tmp.taintedCitationIDs = {};
    var sortedItems = [];

    // retrieve item data and compose items for use in rendering
    // attach pointer to item data to shared copy for good measure
    for (i = 0, ilen = citation.citationItems.length; i < ilen; i += 1) {
        item = citation.citationItems[i];
        if (this.opt.development_extensions.locator_date_and_revision) {
            // Break out locator elements if necessary
            if (item.locator) {
                item.locator = "" + item.locator;
                var idx = item.locator.indexOf("|");
                if (idx > -1) {
                    var raw_locator = item.locator;
                    item.locator = raw_locator.slice(0, idx);
                    raw_locator = raw_locator.slice(idx + 1);
                    var m = raw_locator.match(/^([0-9]{4}-[0-9]{2}-[0-9]{2}).*/);
                    if (m) {
                        item["locator-date"] = this.fun.dateparser.parse(m[1]);
                        raw_locator = raw_locator.slice(m[1].length);
                    }
                    item["locator-revision"] = raw_locator.replace(/^\s+/, "").replace(/\s+$/, "");
                }
            }
        }
        Item = this.retrieveItem("" + item.id);
        var newitem = [Item, item];
        sortedItems.push(newitem);
        citation.citationItems[i].item = Item;
    }

    // ZZZ sort stuff moved from here.

    // attach the sorted list to the citation item
    citation.sortedItems = sortedItems;

    // build reconstituted citations list in current document order
    var citationByIndex = [];
    for (i = 0, ilen = citationsPre.length; i < ilen; i += 1) {
        c = citationsPre[i];
        //CSL.debug("  -- attempting to access Pre citation with ID: ("+c[0]+")");
        this.registry.citationreg.citationById[c[0]].properties.noteIndex = c[1];
        citationByIndex.push(this.registry.citationreg.citationById[c[0]]);
    }
    citationByIndex.push(citation);
    for (i = 0, ilen = citationsPost.length; i < ilen; i += 1) {
        c = citationsPost[i];
        //CSL.debug("  -- attempting to access Post citation with ID: ("+c[0]+")");
        this.registry.citationreg.citationById[c[0]].properties.noteIndex = c[1];
        citationByIndex.push(this.registry.citationreg.citationById[c[0]]);
    }
    this.registry.citationreg.citationByIndex = citationByIndex;

    //
    // The processor provides three facilities to support
    // updates following position reevaluation.
    //
    // (1) The updateItems() function reports tainted ItemIDs
    // to state.tmp.taintedItemIDs.
    //
    // (2) The processor memos the type of style referencing as
    // CSL.NONE, CSL.NUMERIC or CSL.POSITION in state.opt.update_mode.
    //
    // XXXX: NO LONGER
    // (3) For citations containing cites with backreference note numbers,
    // a string image of the rendered citation is held in
    // citation.properties.backref_citation, and a list of
    // ItemIDs to be used to update the backreference note numbers
    // is memoed at citation.properties.backref_index.  When such
    // citations change position, they can be updated with a
    // series of simple find and replace operations, without
    // need for rerendering.
    //

    //
    // Position evaluation!
    //
    // set positions in reconstituted list, noting taints
    this.registry.citationreg.citationsByItemId = {};
    if (this.opt.update_mode === CSL.POSITION) {
        textCitations = [];
        noteCitations = [];
    }
    var update_items = [];
    for (i = 0, ilen = citationByIndex.length; i < ilen; i += 1) {
        citationByIndex[i].properties.index = i;
            for (j = 0, jlen = citationByIndex[i].sortedItems.length; j < jlen; j += 1) {
            item = citationByIndex[i].sortedItems[j];
            if (!this.registry.citationreg.citationsByItemId[item[1].id]) {
                this.registry.citationreg.citationsByItemId[item[1].id] = [];
                update_items.push("" + item[1].id);
            }
            if (this.registry.citationreg.citationsByItemId[item[1].id].indexOf(citationByIndex[i]) === -1) {
                this.registry.citationreg.citationsByItemId[item[1].id].push(citationByIndex[i]);
            }
        }
        if (this.opt.update_mode === CSL.POSITION) {
            if (citationByIndex[i].properties.noteIndex) {
                noteCitations.push(citationByIndex[i]);
            } else {
                textCitations.push(citationByIndex[i]);
            }
        }
    }
    //
    // update bibliography items here
    //
    if (flag !== CSL.ASSUME_ALL_ITEMS_REGISTERED) {
        //SNIP-START
        if (this.debug) {
            CSL.debug("****** start update items *********");
        }
        //SNIP-END
        this.updateItems(update_items);
        //SNIP-START
        if (this.debug) {
            CSL.debug("****** endo update items *********");
        }
        //SNIP-END
    }

    if (!this.opt.citation_number_sort && sortedItems && sortedItems.length > 1 && this.citation_sort.tokens.length > 0) {
        for (i = 0, ilen = sortedItems.length; i < ilen; i += 1) {
            sortedItems[i][1].sortkeys = CSL.getSortKeys.call(this, sortedItems[i][0], "citation_sort");
        }

        /* 
         * Grouped sort stuff (start)
         */

        if (this.opt.grouped_sort &&  !citation.properties.unsorted) {
            // Insert authorstring as key.
            // Note the position of each ID that correspond to the relevant
            // object in the myblobs list.
            for (i = 0, ilen = sortedItems.length; i < ilen; i += 1) {
                var sortkeys = sortedItems[i][1].sortkeys;
                this.tmp.authorstring_request = true;
                // Run getAmbiguousCite() with the current disambig
                // parameters, and pick up authorstring from the registry.
                var mydisambig = this.registry.registry[sortedItems[i][0].id].disambig;
                
                this.tmp.authorstring_request = true;
                CSL.getAmbiguousCite.call(this, sortedItems[i][0], mydisambig);
                var authorstring = this.registry.authorstrings[sortedItems[i][0].id];
                this.tmp.authorstring_request = false;

                sortedItems[i][1].sortkeys = [authorstring].concat(sortkeys);
            }

            sortedItems.sort(this.citation.srt.compareCompositeKeys);
            // Replace authorstring key in items with same (authorstring) with the 
            // keystring of first normal key. This forces grouped sorts,
            // as discussed here:
            // https://github.com/citation-style-language/schema/issues/40
            var lastauthor = false;
            var thiskey = false;
            var thisauthor = false;
            for (i = 0, ilen = sortedItems.length; i < ilen; i += 1) {
                if (sortedItems[i][1].sortkeys[0] !== lastauthor) {
                    thisauthor = sortedItems[i][1].sortkeys[0];
                    thiskey =  sortedItems[i][1].sortkeys[1];
                }
                sortedItems[i][1].sortkeys[0] = "" + thiskey + i;
                lastauthor = thisauthor;
            }
        }
        /*
         * Grouped sort stuff (end)
         */

        if (!citation.properties.unsorted) {
            sortedItems.sort(this.citation.srt.compareCompositeKeys);
        }
    }

    var citations;
    if (this.opt.update_mode === CSL.POSITION) {
        for (i = 0; i < 2; i += 1) {
            citations = [textCitations, noteCitations][i];
            var first_ref = {};
            var last_ref = {};
            for (j = 0, jlen = citations.length; j < jlen; j += 1) {
                var onecitation = citations[j];
                if (!onecitation.properties.noteIndex) {
                    onecitation.properties.noteIndex = 0;
                }
                // Set the following:
                //
                // (1) position as required (as per current Zotero)
                // (2) first-reference-note-number as required (on onecitation item)
                // (3) near-note as required (on onecitation item, according to
                //     state.opt["near-note-distance"] parameter)
                // (4) state.registry.citationreg.citationsByItemId.
                //
                // Any state changes caused by unsetting or resetting should
                // trigger a single entry for the citations in
                // state.tmp.taintedCitationIDs (can block on presence of
                // state.registry.citationreg.citationsByItemId).
                //
                for (k = 0, klen = citations[j].sortedItems.length; k < klen; k += 1) {
                    item = citations[j].sortedItems[k];
                    // Don't touch item data of other cites when previewing
                    if (flag === CSL.PREVIEW) {
                        if (onecitation.citationID != citation.citationID) {
                            if ("undefined" === typeof first_ref[item[1].id]) {
                                first_ref[item[1].id] = onecitation.properties.noteIndex;
                                last_ref[item[1].id] = onecitation.properties.noteIndex;
                            } else {
                                last_ref[item[1].id] = onecitation.properties.noteIndex;
                            }
                            continue;
                        }
                    }
                    var oldvalue = {};
                    oldvalue.position = item[1].position;
                    oldvalue["first-reference-note-number"] = item[1]["first-reference-note-number"];
                    oldvalue["near-note"] = item[1]["near-note"];
                    item[1]["first-reference-note-number"] = 0;
                    item[1]["near-note"] = false;
                    if ("undefined" === typeof first_ref[item[1].id]) {
                        first_ref[item[1].id] = onecitation.properties.noteIndex;
                        last_ref[item[1].id] = onecitation.properties.noteIndex;
                        item[1].position = CSL.POSITION_FIRST;
                    } else {
                        //
                        // backward-looking position evaluation happens here.
                        //
                        //
                        //
                        var ibidme = false;
                        var suprame = false;
                        if (j > 0 && parseInt(k, 10) === 0) {
                            // Case 1: source in previous onecitation
                            // (1) Threshold conditions
                            //     (a) there must be a previous onecitation with one item
                            //     (b) this item must be the first in this onecitation
                            //     (c) the previous onecitation must contain a reference
                            //         to the same item ...
                            //     (d) the note numbers must be the same or consecutive.
                            // (this has some jiggery-pokery in it for parallels)
                            var items = citations[(j - 1)].sortedItems;
                            var useme = false;
                            if ((citations[(j - 1)].sortedItems[0][1].id  == item[1].id && citations[j - 1].properties.noteIndex >= (citations[j].properties.noteIndex - 1)) || citations[(j - 1)].sortedItems[0][1].id == this.registry.registry[item[1].id].parallel) {
                                useme = true;
                            }
                            for (n = 0, nlen = items.slice(1).length; n < nlen; n += 1) {
                                var itmp = items.slice(1)[n];
                                // XXXXX: This test can't be right.  parallel stores an ID ... ?
                                if (!this.registry.registry[itmp[1].id].parallel || this.registry.registry[itmp[1].id].parallel == this.registry.registry[itmp[1].id]) {
                                    // Does fire in some tests, as a matching undefined
                                    // No apparent side effects of turning it off, though.
                                    // For future consideration.
                                    useme = false;
                                }
                            }
                            if (useme) {
                                ibidme = true;
                            } else {
                                suprame = true;
                            }
                        } else if (k > 0 && onecitation.sortedItems[(k - 1)][1].id == item[1].id) {
                            // Case 2: immediately preceding source in this onecitation
                            // (1) Threshold conditions
                            //     (a) there must be an imediately preceding reference to  the
                            //         same item in this onecitation
                            ibidme = true;
                        } else {
                            // everything else is definitely subsequent
                            suprame = true;
                        }
                        // conditions
                        var prev, prev_locator, prev_label, curr_locator, curr_label;
                        if (ibidme) {
                            if (k > 0) {
                                prev = onecitation.sortedItems[(k - 1)][1];
                            } else {
                                prev = citations[(j - 1)].sortedItems[0][1];
                            }
                            if (prev.locator) {
                                if (prev.label) {
                                    prev_label = prev.label;
                                } else {
                                    prev_label = "";
                                }
                                prev_locator = "" + prev.locator + prev_label;
                            } else {
                                prev_locator = prev.locator;
                            }
                            if (item[1].locator) {
                                if (item[1].label) {
                                    curr_label = item[1].label;
                                } else {
                                    curr_label = "";
                                }
                                curr_locator = "" + item[1].locator + curr_label;
                            } else {
                                curr_locator = item[1].locator;
                            }
                        }
                        // triage
                        if (ibidme && prev_locator && !curr_locator) {
                            ibidme = false;
                            suprame = true;

                        }
                        if (ibidme) {
                            if (!prev_locator && curr_locator) {
                                //     (a) if the previous onecitation had no locator
                                //         and this onecitation has one, use ibid+pages
                                item[1].position = CSL.POSITION_IBID_WITH_LOCATOR;
                            } else if (!prev_locator && !curr_locator) {
                                //     (b) if the previous onecitation had no locator
                                //         and this onecitation also has none, use ibid
                                item[1].position = CSL.POSITION_IBID;
                                //print("setting ibid in cmd_cite()");
                            } else if (prev_locator && curr_locator === prev_locator) {
                                //     (c) if the previous onecitation had a locator
                                //         (page number, etc.) and this onecitation has
                                //         a locator that is identical, use ibid

                                item[1].position = CSL.POSITION_IBID;
                                //print("setting ibid in cmd_cite() [2]");
                            } else if (prev_locator && curr_locator && curr_locator !== prev_locator) {
                                //     (d) if the previous onecitation had a locator,
                                //         and this onecitation has one that differs,
                                //         use ibid+pages
                                item[1].position = CSL.POSITION_IBID_WITH_LOCATOR;
                            } else {
                                //     (e) if the previous onecitation had a locator
                                //         and this onecitation has none, use subsequent
                                //
                                //     ... and everything else would be subsequent also
                                ibidme = false; // just to be clear
                                suprame = true;
                            }
                        }
                        if (suprame) {
                            if (this.registry.registry[item[1].id].parallel) {
                                item[1].position = CSL.POSITION_SUBSEQUENT_PARALLEL;
                            } else {
                                item[1].position = CSL.POSITION_SUBSEQUENT;
                            }
                            if (first_ref[item[1].id] != onecitation.properties.noteIndex) {
                                item[1]["first-reference-note-number"] = first_ref[item[1].id];
                            }
                        }
                    }
                    if (onecitation.properties.noteIndex) {
                        var note_distance = parseInt(onecitation.properties.noteIndex, 10) - parseInt(last_ref[item[1].id], 10);
                        if (item[1].position !== CSL.POSITION_FIRST 
                            && note_distance <= this.citation.opt["near-note-distance"]) {
                            item[1]["near-note"] = true;
                        }
                        last_ref[item[1].id] = onecitation.properties.noteIndex;
                    }
                    if (onecitation.citationID != citation.citationID) {
                        for (n = 0, nlen = CSL.POSITION_TEST_VARS.length; n < nlen; n += 1) {
                            var param = CSL.POSITION_TEST_VARS[n];
                            if (item[1][param] !== oldvalue[param]) {
                                this.tmp.taintedCitationIDs[onecitation.citationID] = true;
                            }
                        }
                    }
                }
            }
        }
    }
    if (this.opt.citation_number_sort && sortedItems && sortedItems.length > 1 && this.citation_sort.tokens.length > 0) {
        for (i = 0, ilen = sortedItems.length; i < ilen; i += 1) {
            sortedItems[i][1].sortkeys = CSL.getSortKeys.call(this, sortedItems[i][0], "citation_sort");
        }
        if (!citation.properties.unsorted) {
            sortedItems.sort(this.citation.srt.compareCompositeKeys);
        }
    }
    for (key in this.tmp.taintedItemIDs) {
        if (this.tmp.taintedItemIDs.hasOwnProperty(key)) {
            citations = this.registry.citationreg.citationsByItemId[key];
            // Current citation may be tainted but will not exist
            // during previewing.
            if (citations) {
                for (i = 0, ilen = citations.length; i < ilen; i += 1) {
                    this.tmp.taintedCitationIDs[citations[i].citationID] = true;
                }
            }
        }
    }

    var ret = [];
    if (flag === CSL.PREVIEW) {
        // If previewing, return only a rendered string
        //SNIP-START
        if (this.debug) {
            CSL.debug("****** start run processor *********");
        }
        //SNIP-END
        try {
            ret = this.process_CitationCluster.call(this, citation.sortedItems, citation.citationID);
        } catch (e) {
            CSL.error("Error running CSL processor for preview: "+e);
        }
            
        //SNIP-START
        if (this.debug) {
            CSL.debug("****** end run processor *********");
            CSL.debug("****** start state restore *********");
        }
        //SNIP-END
        // Wind out anything related to new items added for the preview.
        // This means (1) names, (2) disambig state for affected items,
        // (3) keys registered in the ambigs pool arrays, and (4) registry
        // items.
        //

        // restore sliced citations
        this.registry.citationreg.citationByIndex = oldCitationList;
        this.registry.citationreg.citationById = {};
        for (i = 0, ilen = oldCitationList.length; i < ilen; i += 1) {
            this.registry.citationreg.citationById[oldCitationList[i].citationID] = oldCitationList[i];
        }

        //SNIP-START
        if (this.debug) {
            CSL.debug("****** start final update *********");
        }
        //SNIP-END
        var oldItemIds = [];
        for (i = 0, ilen = oldItemList.length; i < ilen; i += 1) {
            oldItemIds.push("" + oldItemList[i].id);
        }
        this.updateItems(oldItemIds);
        //SNIP-START
        if (this.debug) {
            CSL.debug("****** end final update *********");
        }
        //SNIP-END
        // Roll back disambig states
        for (key in oldAmbigs) {
            if (oldAmbigs.hasOwnProperty(key)) {
                this.registry.registry[key].disambig = oldAmbigs[key];
            }
        }
        //SNIP-START
        if (this.debug) {
            CSL.debug("****** end state restore *********");
        }
        //SNIP-END
    } else {
        // Run taints only if not previewing
        //
        // Push taints to the return object
        //
        var obj;
        for (key in this.tmp.taintedCitationIDs) {
            if (this.tmp.taintedCitationIDs.hasOwnProperty(key)) {
                if (key == citation.citationID) {
                    continue;
                }
                var mycitation = this.registry.citationreg.citationById[key];
                // For error reporting
                this.tmp.citation_pos = mycitation.properties.index;
                this.tmp.citation_note_index = mycitation.properties.noteIndex;
                this.tmp.citation_id = "" + mycitation.citationID;
                obj = [];
                obj.push(mycitation.properties.index);
                obj.push(this.process_CitationCluster.call(this, mycitation.sortedItems, mycitation.citationID));
                ret.push(obj);
                this.tmp.citation_pos += 1;
            }
        }
        this.tmp.taintedItemIDs = false;
        this.tmp.taintedCitationIDs = false;

        // For error reporting again
        this.tmp.citation_pos = citation.properties.index;
        this.tmp.citation_note_index = citation.properties.noteIndex;
        this.tmp.citation_id = "" + citation.citationID;

        obj = [];
        obj.push(citationsPre.length);
        obj.push(this.process_CitationCluster.call(this, sortedItems));
        ret.push(obj);
        //
        // note for posterity: Rhino and Spidermonkey produce different
        // sort results for items with matching keys.  That discrepancy
        // turned up a subtle bug in the parallel detection code, trapped
        // at line 266, above, and in line 94 of util_parallel.js.
        //
        ret.sort(function (a, b) {
            if (a[0] > b[0]) {
                return 1;
            } else if (a[0] < b[0]) {
                return -1;
            } else {
                return 0;
            }
        });
        //
        // In normal rendering, return is a list of two-part arrays, with the first element
        // a citation index number, and the second the text to be inserted.
        //
    }
    return_data.citation_errors = this.tmp.citation_errors.slice();
    return [return_data, ret];
};

CSL.Engine.prototype.process_CitationCluster = function (sortedItems, citationID) {
    var str;
    this.parallel.StartCitation(sortedItems);
    str = CSL.getCitationCluster.call(this, sortedItems, citationID);

    return str;
};

CSL.Engine.prototype.makeCitationCluster = function (rawList) {
    var inputList, newitem, str, pos, len, item, Item;
    inputList = [];
    len = rawList.length;
    for (pos = 0; pos < len; pos += 1) {
        item = rawList[pos];
        Item = this.retrieveItem("" + item.id);
        newitem = [Item, item];
        inputList.push(newitem);
    }
    if (inputList && inputList.length > 1 && this.citation_sort.tokens.length > 0) {
        len = inputList.length;
        for (pos = 0; pos < len; pos += 1) {
            rawList[pos].sortkeys = CSL.getSortKeys.call(this, inputList[pos][0], "citation_sort");
        }
        inputList.sort(this.citation.srt.compareCompositeKeys);
    }
    this.tmp.citation_errors = [];
    this.parallel.StartCitation(inputList);
    str = CSL.getCitationCluster.call(this, inputList);
    return str;
};


/**
 * Get the undisambiguated version of a cite, without decorations
 * <p>This is used internally by the Registry.</p>
 */
CSL.getAmbiguousCite = function (Item, disambig) {
    var use_parallels, ret;
    var oldTermSiblingLayer = this.tmp.group_context.value().slice();
    if (disambig) {
        this.tmp.disambig_request = disambig;
    } else {
        this.tmp.disambig_request = false;
    }
    this.tmp.area = "citation";
    use_parallels = this.parallel.use_parallels;
    this.parallel.use_parallels = false;
    this.tmp.suppress_decorations = true;
    this.tmp.just_looking = true;
    CSL.getCite.call(this, Item, {position: 1});
    // !!!
    CSL.Output.Queue.purgeEmptyBlobs(this.output.queue);
    if (this.opt.development_extensions.clean_up_csl_flaws) {
        CSL.Output.Queue.adjustPunctuation(this, this.output.queue);
    }
    ret = this.output.string(this, this.output.queue);
    this.tmp.just_looking = false;
    this.tmp.suppress_decorations = false;
    this.parallel.use_parallels = use_parallels;
    // Cache the result.
    this.tmp.group_context.replace(oldTermSiblingLayer, "literal");
    return ret;
};

/**
 * Return delimiter for use in join
 * <p>Splice evaluation is done during cite
 * rendering, and this method returns the
 * result.  Evaluation requires three items
 * of information from the preceding cite, if
 * one is present: the names used; the years
 * used; and the suffix appended to the
 * citation.  These details are copied into
 * the state object before processing begins,
 * and are cleared by the processor on
 * completion of the run.</p>
 */

CSL.getSpliceDelimiter = function (last_collapsed, pos) {
    if (last_collapsed && ! this.tmp.have_collapsed && "string" === typeof this.citation.opt["after-collapse-delimiter"]) {
        this.tmp.splice_delimiter = this.citation.opt["after-collapse-delimiter"];
    } else if (this.tmp.have_collapsed && this.opt.xclass === "in-text" && this.opt.update_mode !== CSL.NUMERIC) {
        this.tmp.splice_delimiter = ", ";
    } else if (this.tmp.cite_locales[pos - 1]) {
        //
        // Must have a value to take effect.  Use zero width space to force empty delimiter.
        var alt_affixes = this.tmp.cite_affixes[this.tmp.cite_locales[pos - 1]];
        if (alt_affixes && alt_affixes.delimiter) {
            this.tmp.splice_delimiter = alt_affixes.delimiter;
        }
    }
    // Paranoia
    if (!this.tmp.splice_delimiter) {
        this.tmp.splice_delimiter = "";
    }
    return this.tmp.splice_delimiter;
};

/*
 * Compose individual cites into a single string, with
 * flexible inter-cite splicing.
 */
CSL.getCitationCluster = function (inputList, citationID) {
    var result, objects, myparams, len, pos, item, last_collapsed, params, empties, composite, compie, myblobs, Item, llen, ppos, obj, preceding_item, txt_esc, error_object;
    this.tmp.last_primary_names_string = false;
    txt_esc = CSL.Output.Formats[this.opt.mode].text_escape;
    this.tmp.area = "citation";
    result = "";
    objects = [];
    this.tmp.last_suffix_used = "";
    this.tmp.last_names_used = [];
    this.tmp.last_years_used = [];
    this.tmp.backref_index = [];
    this.tmp.cite_locales = [];
    if (citationID) {
        this.registry.citationreg.citationById[citationID].properties.backref_index = false;
        this.registry.citationreg.citationById[citationID].properties.backref_citation = false;
    }

    myparams = [];
    len = inputList.length;
    for (pos = 0; pos < len; pos += 1) {
        Item = inputList[pos][0];
        item = inputList[pos][1];
        last_collapsed = this.tmp.have_collapsed;
        params = {};

        if (pos > 0) {
            CSL.getCite.call(this, Item, item, "" + inputList[(pos - 1)][0].id);
        } else {
            this.tmp.term_predecessor = false;
            CSL.getCite.call(this, Item, item);
        }
        // Make a note of any errors
        if (!this.tmp.cite_renders_content) {
            error_object = {
                citationID: "" + this.tmp.citation_id,
                index: this.tmp.citation_pos,
                noteIndex: this.tmp.citation_note_index,
                itemID: "" + Item.id,
                citationItems_pos: pos,
                error_code: CSL.ERROR_NO_RENDERED_FORM
            };
            this.tmp.citation_errors.push(error_object);
        }
        if (pos === (inputList.length - 1)) {
            this.parallel.ComposeSet();
        }
        params.splice_delimiter = CSL.getSpliceDelimiter.call(this, last_collapsed, pos);
        if (item && item["author-only"]) {
            this.tmp.suppress_decorations = true;
        }

        if (pos > 0) {
            preceding_item = inputList[pos - 1][1];
            if (preceding_item.suffix && pos > 0 && preceding_item.suffix.slice(-1) === ".") {
                var spaceidx = params.splice_delimiter.indexOf(" ");
                if (spaceidx > -1) {
                    params.splice_delimiter = params.splice_delimiter.slice(spaceidx);
                } else {
                    params.splice_delimiter = "";
                }
            }
        }
        params.suppress_decorations = this.tmp.suppress_decorations;
        params.have_collapsed = this.tmp.have_collapsed;
        //
        // XXXXX: capture parameters to an array, which
        // will be of the same length as this.output.queue,
        // corresponding to each element.
        //
        myparams.push(params);
    }

    this.parallel.PruneOutputQueue(this);
    //
    // output.queue is a simple array.  do a slice
    // of it to get each cite item, setting params from
    // the array that was built in the preceding loop.
    //
    empties = 0;
    myblobs = this.output.queue.slice();

    // Use a fake blob to reflect any mods to the suffix and delimiter
    var fakeblob = {
        strings: {
            suffix: this.citation.opt.layout_suffix,
            delimiter: this.citation.opt.layout_delimiter                
        }
    };
    var suffix = this.citation.opt.layout_suffix;
    var last_locale = this.tmp.cite_locales[this.tmp.cite_locales.length - 1];
    //
    // Must have a value to take effect.  Use zero width space to force empty suffix.
    if (last_locale && this.tmp.cite_affixes[last_locale] && this.tmp.cite_affixes[last_locale].suffix) {
        suffix = this.tmp.cite_affixes[last_locale].suffix;
    }
    if (CSL.TERMINAL_PUNCTUATION.slice(0, -1).indexOf(suffix.slice(0, 1)) > -1) {
        suffix = suffix.slice(0, 1);
    }
    var delimiter = this.citation.opt.layout_delimiter;
    if (!delimiter) {
        delimiter = "";
    }
    if (CSL.TERMINAL_PUNCTUATION.slice(0, -1).indexOf(delimiter.slice(0, 1)) > -1) {
        delimiter = delimiter.slice(0, 1);
    }
    var mystk = [
        {
            suffix: "",
            delimiter: delimiter,
            blob: fakeblob
        }
    ];
    //print("=== FROM CITE ===");
    var use_layout_suffix = suffix;
    
    for (pos = 0, len = myblobs.length; pos < len; pos += 1) {
        CSL.Output.Queue.purgeEmptyBlobs(this.output.queue, true);
    }
    for (pos = 0, len = myblobs.length; pos < len; pos += 1) {
        this.output.queue = [myblobs[pos]];

        this.tmp.suppress_decorations = myparams[pos].suppress_decorations;
        
        this.tmp.splice_delimiter = myparams[pos].splice_delimiter;
        //
        // oh, one last second thought on delimiters ...
        //
        if (myblobs[pos].parallel_delimiter) {
            this.tmp.splice_delimiter = myblobs[pos].parallel_delimiter;
        }
        this.tmp.have_collapsed = myparams[pos].have_collapsed;

        // No purgeEmptyBlobs() with this housecleaning adjustment
        // to punctuation.
        if (pos === (myblobs.length - 1)) {
            mystk[0].suffix = use_layout_suffix;
        }
        if (this.opt.development_extensions.clean_up_csl_flaws) {
            CSL.Output.Queue.adjustPunctuation(this, this.output.queue, mystk);
        }
        composite = this.output.string(this, this.output.queue);
        this.tmp.suppress_decorations = false;
        // meaningless assignment
        // this.tmp.handle_ranges = false;
        if ("string" === typeof composite) {
            this.tmp.suppress_decorations = false;
            return composite;
        }
        if ("object" === typeof composite && composite.length === 0 && !item["suppress-author"]) {
            composite.push("[CSL STYLE ERROR: reference with no printed form.]");
        }
        if (objects.length && "string" === typeof composite[0]) {
            composite.reverse();
            var tmpstr = composite.pop();
            if (tmpstr && tmpstr.slice(0, 1) === ",") {
                objects.push(tmpstr);
            } else {
                objects.push(txt_esc(this.tmp.splice_delimiter) + tmpstr);
            }
        } else {
            composite.reverse();
            compie = composite.pop();
            if ("undefined" !== typeof compie) {
                if (objects.length && "string" === typeof objects[objects.length - 1]) {
                    objects[objects.length - 1] += compie.successor_prefix;
                }
                objects.push(compie);
            }
        }
        // Seems odd, but this was unnecessary and broken.
        //composite.reverse();
        llen = composite.length;
        for (ppos = 0; ppos < llen; ppos += 1) {
            obj = composite[ppos];
            if ("string" === typeof obj) {
                objects.push(txt_esc(this.tmp.splice_delimiter) + obj);
                continue;
            }
            compie = composite.pop();
            if ("undefined" !== typeof compie) {
                objects.push(compie);
            }
        }
        if (objects.length === 0 && !inputList[pos][1]["suppress-author"]) {
            empties += 1;
        }
    }
    result += this.output.renderBlobs(objects);
    if (result) {
        if (CSL.TERMINAL_PUNCTUATION.indexOf(this.tmp.last_chr) > -1 
            && this.tmp.last_chr === use_layout_suffix.slice(0, 1)) {
            use_layout_suffix = use_layout_suffix.slice(1);
        }
        result = txt_esc(this.citation.opt.layout_prefix) + result + txt_esc(use_layout_suffix);
        if (!this.tmp.suppress_decorations) {
            len = this.citation.opt.layout_decorations.length;
            for (pos = 0; pos < len; pos += 1) {
                params = this.citation.opt.layout_decorations[pos];
                // The "normal" formats in some output modes expect
                // a superior nested decoration environment, and
                // so should produce no output here.
                if (params[1] === "normal") {
                    continue;
                }
                result = this.fun.decorate[params[0]][params[1]](this, result);
            }
        }
    }
    this.tmp.suppress_decorations = false;
    return result;
};

/*
 * Render a single cite item.
 *
 * This is called on the state object, with a single
 * Item as input.  It iterates exactly once over the style
 * citation tokens, and leaves the result of rendering in
 * the top-level list in the relevant *.opt.output
 * stack, as a list item consisting of a single string.
 *
 * (This is dual-purposed for generating individual
 * entries in a bibliography.)
 */
CSL.getCite = function (Item, item, prevItemID) {
    var next, error_object;
    this.tmp.cite_renders_content = false;
    this.parallel.StartCite(Item, item, prevItemID);
    CSL.citeStart.call(this, Item, item);
    next = 0;
    this.nameOutput = new CSL.NameOutput(this, Item, item);
    while (next < this[this.tmp.area].tokens.length) {
        next = CSL.tokenExec.call(this, this[this.tmp.area].tokens[next], Item, item);
    }
    CSL.citeEnd.call(this, Item, item);
    this.parallel.CloseCite(this);
    // Odd place for this, but it seems to fit here
    if (!this.tmp.cite_renders_content && !this.tmp.just_looking) {
        if (this.tmp.area === "bibliography") {
            error_object = {
                index: this.tmp.bibliography_pos,
                itemID: "" + Item.id,
                error_code: CSL.ERROR_NO_RENDERED_FORM
            };
            this.tmp.bibliography_errors.push(error_object);
        }
    }
    return "" + Item.id;
};


CSL.citeStart = function (Item, item) {
    this.tmp.same_author_as_previous_cite = false;
    this.tmp.lastchr = "";
    if (this.tmp.area === "citation" && this.citation.opt.collapse && this.citation.opt.collapse.length) {
        this.tmp.have_collapsed = true;
    } else {
        this.tmp.have_collapsed = false;
    }
    this.tmp.render_seen = false;
    if (this.tmp.disambig_request  && ! this.tmp.disambig_override) {
        this.tmp.disambig_settings = this.tmp.disambig_request;
    } else if (this.registry.registry[Item.id] && ! this.tmp.disambig_override) {
        this.tmp.disambig_request = this.registry.registry[Item.id].disambig;
        this.tmp.disambig_settings = this.registry.registry[Item.id].disambig;
    } else {
        this.tmp.disambig_settings = new CSL.AmbigConfig();
    }
    if (this.tmp.area === 'bibliography' && this.opt["disambiguate-add-names"] && this.registry.registry[Item.id] && this.tmp.disambig_override) {
        this.tmp.disambig_request = this.tmp.disambig_settings;
        this.tmp.disambig_request.names = this.registry.registry[Item.id].disambig.names.slice();
        this.tmp.disambig_settings.names = this.registry.registry[Item.id].disambig.names.slice();
    }
    this.tmp.names_used = [];
    this.tmp.nameset_counter = 0;
    this.tmp.years_used = [];
    this.tmp.names_max.clear();

    this.tmp.splice_delimiter = this[this.tmp.area].opt.layout_delimiter;
    //this.tmp.splice_delimiter = this[this.tmp.area].opt.delimiter;

    this.bibliography_sort.keys = [];
    this.citation_sort.keys = [];

    this.tmp.has_done_year_suffix = false;
    this.tmp.last_cite_locale = false;
    // SAVE PARAMETERS HERE, IF APPROPRIATE
    // (promiscuous addition of global parameters => death by a thousand cuts)
    if (!this.tmp.just_looking && item && !item.position && this.registry.registry[Item.id]) {
        this.tmp.disambig_restore = CSL.cloneAmbigConfig(this.registry.registry[Item.id].disambig);
    }
    this.tmp.shadow_numbers = {};
    this.tmp.first_name_string = false;
};

CSL.citeEnd = function (Item, item) {
    // RESTORE PARAMETERS IF APPROPRIATE
    if (this.tmp.disambig_restore) {
        this.registry.registry[Item.id].disambig.names = this.tmp.disambig_restore.names;
        this.registry.registry[Item.id].disambig.givens = this.tmp.disambig_restore.givens;
    }
    this.tmp.disambig_restore = false;

    this.tmp.last_suffix_used = this.tmp.suffix.value();
    this.tmp.last_years_used = this.tmp.years_used.slice();
    this.tmp.last_names_used = this.tmp.names_used.slice();
    this.tmp.cut_var = false;

    // This is a hack, in a way; I have lost track of where
    // the disambig (name rendering) settings used for rendering work their way
    // into the registry.  This resets defaults to the subsequent form,
    // when first cites are rendered.
    //if (this.tmp.disambig_restore && this.registry.registry[Item.id]) {
    //    this.registry.registry[Item.id].disambig = this.tmp.disambig_restore;
    //}
    //this.tmp.disambig_restore = false;
    this.tmp.disambig_request = false;

    this.tmp.cite_locales.push(this.tmp.last_cite_locale);
};


// More on personal names, I think
// This should be run after all names have been sent to
// output, no? It's used to determine what join should be
// applied -- maybe that itself is not necessary, since
// alternative joins are really only needed for author-date
// styles, in which case we'd be looking at collapsing
// anyway.
//
// I seem to remember that the collapse toggle raises too late,
// though.
/*
                    if (!state.tmp.suppress_decorations
                        && state.tmp.last_names_used.length === state.tmp.names_used.length
                        && state.tmp.area === "citation") {
                        // lastones = state.tmp.last_names_used[state.tmp.nameset_counter];
                        lastones = state.tmp.last_names_used[state.tmp.nameset_counter];
                        //lastones = state.tmp.last_names_used;
                        currentones = state.tmp.names_used[state.tmp.nameset_counter];
                        //currentones = state.tmp.names_used;
                        compset = [currentones, lastones];
                        if (CSL.Util.Names.compareNamesets(lastones,currentones)) {
                            state.tmp.same_author_as_previous_cite = true;
                        }
                    }

                    if (!state.tmp.suppress_decorations && (state[state.tmp.area].opt.collapse === "year" || state[state.tmp.area].opt.collapse === "year-suffix" || state[state.tmp.area].opt.collapse === "year-suffix-ranged")) {
                        //
                        // This is fine, but the naming of the comparison
                        // function is confusing.  This is just checking whether the
                        // current name is the same as the last name rendered
                        // in the last cite, and it works.  Set a toggle if the
                        // test fails, so we can avoid further suppression in the
                        // cite.
                        //

                        //if (state.tmp.last_names_used.length === state.tmp.names_used.length) {
                        if (state.tmp.same_author_as_previous_cite) {
                            continue;
                        } else {
                            state.tmp.have_collapsed = false;
                        }
                    } else {
                        state.tmp.have_collapsed = false;
                    }

 */
