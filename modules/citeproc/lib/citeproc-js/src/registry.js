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


//
// Time for a rewrite of this module.
//
// Simon has pointed out that list and hash behavior can
// be obtained by ... just using a list and a hash.  This
// is faster for batched operations, because sorting is
// greatly optimized.  Since most of the interaction
// with plugins at runtime will involve batches of
// references, there will be solid gains if the current,
// one-reference-at-a-time approach implemented here
// can be replaced with something that leverages the native
// sort method of the Array() type.
//
// That's going to take some redesign, but it will simplify
// things in the long run, so it might as well happen now.
//
// We'll keep makeCitationCluster and makeBibliography as
// simple methods that return a string.  Neither should
// have any effect on internal state.  This will be a change
// in behavior for makeCitationCluster.
//
// A new updateItems command will be introduced, to replace
// insertItems.  It will be a simple list of IDs, in the
// sequence of first reference in the document.
//
// The calling application should always invoke updateItems
// before makeCitationCluster.
//

//
// should allow batched registration of items by
// key.  should behave as an update, with deletion
// of items and the tainting of disambiguation
// partner sets affected by a deletes and additions.
//
//
// we'll need a reset method, to clear the decks
// in the citation area and start over.

/**
 * Registry of cited items.
 * <p>This is a persistent store of disambiguation and
 * sort order information relating to individual items
 * for which rendering is requested.  Item data is stored
 * in a hash, with the item key as hash key, for quick
 * retrieval.  A virtual sequence within the hashed store
 * is maintained on the fly as items are added to the
 * store, using <code>*_next</code> and <code>*_prev</code>
 * attributes on each item.  A separate hash of items
 * based on their undisambiguated cite form is
 * maintained, and the item id list and disambiguation
 * level for each set of disambiguation partners is shared
 * through the registry item.</p>
 * @class
 */
CSL.Registry = function (state) {
    var pos, len, ret, i, ilen;
    this.debug = false;
    this.state = state;
    this.registry = {};
    this.reflist = [];
    this.namereg = new CSL.Registry.NameReg(state);
    this.citationreg = new CSL.Registry.CitationReg(state);
    // See CSL.NameOutput.prototype.outputNames
    // and CSL.Registry.prototype.doinserts
    this.authorstrings = {};

    this.generate = {};
    this.generate.origIDs = {};
    this.generate.genIDs = {};
    this.generate.rules = [];

    //
    // shared scratch vars
    this.mylist = [];
    this.myhash = {};
    this.deletes = [];
    this.inserts = [];
    this.uncited = [];
    this.refreshes = {};
    this.akeys = {};
    this.oldseq = {};
    this.return_data = {};
    //
    // each ambig is a list of the ids of other objects
    // that have the same base-level rendering
    this.ambigcites = {};
    this.sorter = new CSL.Registry.Comparifier(state, "bibliography_sort");
    //this.modes = CSL.getModes.call(this.state);
    //this.checkerator = new CSL.Checkerator();

    this.getSortedIds = function () {
        ret = [];
        for (i = 0, ilen = this.reflist.length; i < ilen; i += 1) {
            ret.push("" + this.reflist[i].id);
        }
        return ret;
    };

    this.getSortedRegistryItems = function () {
        ret = [];
        for (i = 0, ilen = this.reflist.length; i < ilen; i += 1) {
            ret.push(this.reflist[i]);
        }
        return ret;
    };
};

//
// Here's the sequence of operations to be performed on
// update:
//
//  1.  (o) [init] Receive list as function argument, store as hash and as list.
//  2.  (o) [init] Initialize refresh list.  Never needs sorting, only hash required.

//  3.  (o) [dodeletes] Delete loop.
//  3a. (o) [dodeletes] Delete names in items to be deleted from names reg.
//  3b. (o) [dodeletes] Complement refreshes list with items affected by
//      possible name changes.  We'll actually perform the refresh once
//      all of the necessary data and parameters have been established
//      in the registry.
//  3c. (o) [dodeletes] Delete all items to be deleted from their disambig pools.
//  3d. (o) [dodeletes] Delete all items in deletion list from hash.

//  4.  (o) [doinserts] Insert loop.
//  4a. (o) [doinserts] Retrieve entries for items to insert.
//  4b. (o) [doinserts] Generate ambig key.
//  4c. (o) [doinserts] Add names in items to be inserted to names reg
//      (implicit in getAmbiguousCite).
//  4d. (o) [doinserts] Record ambig pool key on akey list (used for updating further
//      down the chain).
//  4e. (o) [doinserts] Create registry token.
//  4f. (o) [doinserts] Add item ID to hash.
//  4g. (o) [doinserts] Set and record the base token to hold disambiguation
//      results ("disambig" in the object above).
//  5.  (o) [rebuildlist] Create "new" list of hash pointers, in the order given
//          in the argument to the update function.
//  6.  (o) [rebuildlist] Apply citation numbers to new list.
//  7.  (o) [dorefreshes] Refresh items requiring update.



//  5. (o) [delnames] Delete names in items to be deleted from names reg, and obtain IDs
//         of other items that would be affected by changes around that surname.
//  6. (o) [delnames] Complement delete and insert lists with items affected by
//         possible name changes.
//  7. (o) [delambigs] Delete all items to be deleted from their disambig pools.
//  8. (o) [delhash] Delete all items in deletion list from hash.

//  9. (o) [addtohash] Retrieve entries for items to insert.
// 10. (o) [addtohash] Add items to be inserted to their disambig pools.
// 11. (o) [addtohash] Add names in items to be inserted to names reg
//         (implicit in getAmbiguousCite).
// 12. (o) [addtohash] Create registry token for each item to be inserted.
// 13. (o) [addtohash] Add items for insert to hash.

// 14. (o) [buildlist] Create "new" list of hash pointers, in the order given in the argument
//         to the update function.
// 15. (o) [renumber] Apply citation numbers to new list.
// 16. (o) [setdisambigs] Set disambiguation parameters on each inserted item token.
// 17. (o) [setsortkeys] Set sort keys on each item token.
// 18. (o) [sorttokens] Resort token list
// 19. (o) [renumber] Reset citation numbers on list items
//

CSL.Registry.prototype.init = function (myitems, uncited_flag) {
    var i, ilen;
    this.oldseq = {};
	//  0. Before doing anything, purge any duplicate items from
	//     the list. Avoids the issue reported here:
	//         https://bitbucket.org/fbennett/citeproc-js/issue/132/differences-in-behaviour-from
	var tmphash = {};
	for (i = myitems.length - 1; i > -1; i += -1) {
		if (tmphash[myitems[i]]) {
			myitems = myitems.slice(0, i).concat(myitems.slice(i + 1));
		} else {
			tmphash[myitems[i]] = true;
		}
	}
    //
    //  1. Receive list as function argument, store as hash and as list.
    //
    // (be additive if only marking uncited items)
    if (uncited_flag && this.mylist && this.mylist.length) {
        this.uncited = myitems;
        for (i = 0, ilen = myitems.length; i < ilen; i += 1) {
            // XXXX Checking whether this is where the extra copy of uncited IDs creeps in.
            // Confirmed: it is
            // This could be more efficient, but for the time being,
            // refresh following a manual insert to the bibliography works
            // as expected.
            myitems[i] = "" + myitems[i];
            if (!this.myhash[myitems[i]] && this.mylist.indexOf(myitems[i]) === -1) {
                this.mylist.push(myitems[i]);
            }
        }
    } else {
        this.mylist = myitems.concat(this.uncited);
    }
    this.myhash = {};
    for (i = 0, ilen = this.mylist.length; i < ilen; i += 1) {
        this.mylist[i] = "" + this.mylist[i];
        this.myhash[this.mylist[i]] = true;
    }
    //
    //  2. Initialize refresh list.  Never needs sorting, only hash required.
    //
    this.refreshes = {};
    this.touched = {};
};

CSL.Registry.prototype.dodeletes = function (myhash) {
    var otheritems, key, ambig, pos, len, items, kkey, mypos, id;
    if ("string" === typeof myhash) {
        myhash = {};
        myhash[myhash] = true;
    }
    //
    //  3. Delete loop.
    //
    for (key in this.registry) {
        if (this.registry.hasOwnProperty(key) && !myhash[key]) {
            // skip items explicitly marked as uncited
            if (this.registry[key].uncited) {
                continue;
            }
            //
            //  3a. Delete names in items to be deleted from names reg.
            //
            otheritems = this.namereg.delitems(key);
            //
            //  3b. Complement refreshes list with items affected by
            //      possible name changes.  We'll actually perform the refresh once
            //      all of the necessary data and parameters have been established
            //      in the registry.
            //
            for (kkey in otheritems) {
                if (otheritems.hasOwnProperty(kkey)) {
                    this.refreshes[kkey] = true;
                }
            }
            //
            //  3c. Delete all items to be deleted from their disambig pools.
            //
            ambig = this.registry[key].ambig;
            mypos = this.ambigcites[ambig].indexOf(key);
            if (mypos > -1) {
                items = this.ambigcites[ambig].slice();
                this.ambigcites[ambig] = items.slice(0, mypos).concat(items.slice([(mypos + 1)], items.length));
            }
            //
            // XX. What we've missed is to provide an update of all
            // items sharing the same ambig  += -1 the remaining items in
            // ambigcites.  So let's do that here, just in case the
            // names update above doesn't catch them all.
            //
            len = this.ambigcites[ambig].length;
            for (pos = 0; pos < len; pos += 1) {
                id = "" + this.ambigcites[ambig][pos];
                this.refreshes[id] = true;
            }
            //
            //  3d. Delete all items in deletion list from hash.
            //
            delete this.registry[key];
            // + delete any generation rule bundles associated with the item.
            if (this.generate.origIDs[key]) {
                delete this.generate.origIDs[key];
                delete this.generate.genIDs[key + ":gen"];
            }
            // For processCitationCluster()
            this.return_data.bibchange = true;
        }
    }
    // Disabled.  See formats.js for code.
    // this.state.fun.decorate.items_delete( this.state.output[this.state.opt.mode].tmp, myhash );
};

CSL.Registry.prototype.doinserts = function (mylist) {
    var len, pos, item, Item, akey, newitem, abase, j, jlen, k, klen, i, ilen;
    if ("string" === typeof mylist) {
        mylist = [mylist];
    }
    //
    //  4. Insert loop.
    //
    for (i = 0, ilen = mylist.length; i < ilen; i += 1) {
        item = mylist[i];
        if (!this.registry[item]) {
            //
            //  4a. Retrieve entries for items to insert.
            //
            Item = this.state.retrieveItem(item);

            // Add a generation rule for this item if appropriate
            for (j = 0, jlen = this.generate.rules.length; j < jlen; j += 1) {
                if (Item.type === this.generate.rules[j].from) {
                    var needsRule = true;
                    for (k = 0, klen = this.generate.rules[j].triggers.length; k < klen; k += 1) {
                        if (!Item[this.generate.rules[j].triggers[k]]) {
                            needsRule = false;
                            break;
                        }
                    }
                    if (needsRule) {
                        this.generate.origIDs[item] = this.generate.rules[j];
                        this.generate.genIDs[item + ":gen"] = this.generate.rules[j];
                    }
                }
            }

            // If getAbbreviation is available, run it over any
            // relevant fields.
            if (this.state.sys.getAbbreviation) {
                for (var field in this.state.transform.abbrevs["default"]) {
                    
                    switch (field) {
                    case "place":
                        if (Item["publisher-place"]) {
                            this.state.transform.loadAbbreviation(Item.jurisdiction, "place", Item["publisher-place"]);
                        } else if (Item["event-place"]) {
                            this.state.transform.loadAbbreviation(Item.jurisdiction, "place", Item["event-place"]);
                        }
                        break;
                        
                    case "institution-part":
                        for (var creatorVar in CSL.CREATORS) {
                            for (var creatorList in Item[creatorVar]) {
                                for (j = 0, jlen = creatorList.length; j < jlen; j += 1) {
                                    if (creatorList[j].isInstitution) {
                                        var subOrganizations = creatorList[j].literal;
                                        if (!subOrganizations) {
                                            subOrganizations = creatorList[j].family;
                                        }
                                        if (subOrganizations) {
                                            subOrganizations = subOrganizations.split(/\s*|\s*/);
                                            for (k = 0, klen = subOrganizations.length; k < klen; k += 1) {
                                                this.state.transform.loadAbbreviation(Item.jurisdiction, "institution-part", subOrganizations[k]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            break;

                        default:
                            if (Item[field]) {
                                this.state.transform.loadAbbreviation(Item.jurisdiction, field, Item[field]);
                            }
                            break;

                        }
                    }
            }
            //
            //  4b. Generate ambig key.
            //
            // AND
            //
            //  4c. Add names in items to be inserted to names reg
            //      (implicit in getAmbiguousCite).
            //
            akey = CSL.getAmbiguousCite.call(this.state, Item);
            //
            //  4d. Record ambig pool key on akey list (used for updating further
            //      down the chain).
            //
            this.akeys[akey] = true;
            //
            //  4e. Create registry token.
            //
            newitem = {
                "id": "" + item,
                "seq": 0,
                "offset": 0,
                "sortkeys": false,
                "ambig": false,
                "rendered": false,
                "disambig": false,
                "ref": Item
            };
            //
            //
            //  4f. Add item ID to hash.
            //
            this.registry[item] = newitem;
            //
            //  4g. Set and record the base token to hold disambiguation
            //      results ("disambig" in the object above).
            //
            abase = CSL.getAmbigConfig.call(this.state);
            this.registerAmbigToken(akey, item, abase);

            //if (!this.ambigcites[akey]){
            //    this.ambigcites[akey] = [];
            //}
            //CSL.debug("Run: "+item+"("+this.ambigcites[akey]+")");
            //if (this.ambigcites[akey].indexOf(item) === -1){
            //    CSL.debug("  Add: "+item);
            //    this.ambigcites[akey].push(item);
            //}
            //
            //  4h. Make a note that this item needs its sort keys refreshed.
            //
            this.touched[item] = true;
            // For processCitationCluster()
            this.return_data.bibchange = true;
        }
    }
    // Disabled.  See formats.js for code.
    // this.state.fun.decorate.items_add( this.state.output[this.state.opt.mode].tmp, mylist );
};

CSL.Registry.prototype.douncited = function () {
    var pos, len;
    for (pos = 0, len = this.mylist.length; pos < len; pos += 1) {
        this.registry[this.mylist[pos]].uncited = false;
    }
    for (pos = 0, len = this.uncited.length; pos < len; pos += 1) {
        this.registry[this.mylist[pos]].uncited = true;
    }
};

CSL.Registry.prototype.rebuildlist = function () {
    var count, len, pos, item;
    //
    //  5. Create "new" list of hash pointers, in the order given in the argument
    //     to the update function.
    //
    this.reflist = [];
    //
    //  6. Apply citation numbers to new list,
    //     saving off old sequence numbers as we go.
    //
    // count = 1;
    if (this.state.opt.citation_number_sort_direction === CSL.DESCENDING
       && this.state.opt.citation_number_sort_used) {
        //this.mylist.reverse();
    }
    len = this.mylist.length;
    for (pos = 0; pos < len; pos += 1) {
        item = this.mylist[pos];
        this.reflist.push(this.registry[item]);
        this.oldseq[item] = this.registry[item].seq;
        this.registry[item].seq = (pos + 1);
        // count += 1;
    }
    if (this.state.opt.citation_number_sort_direction === CSL.DESCENDING
       && this.state.opt.citation_number_sort_used) {
        //this.mylist.reverse();
    }
};

/*
 * Okay, at this point we should have a numbered list
 * of registry tokens in the notional order requested,
 * with sequence numbers to reconstruct the ordering
 * if the list is remangled.  So far so good.
 */

CSL.Registry.prototype.dorefreshes = function () {
    var key, regtoken, Item, old_akey, akey, abase;
    //
    //  7. Refresh items requiring update.
    //
    // It looks like we need to do four things on each cite for refresh:
    // (1) Generate the akey for the cite.
    // (2) Register it on the ambig token.
    // (3) Register the akey in this.akeys
    // (4) Register the item ID in this.touched
    //
    for (key in this.refreshes) {
        if (this.refreshes.hasOwnProperty(key)) {
            regtoken = this.registry[key];
            delete this.registry[key];
            if (!regtoken) {
                continue;
            }
            regtoken.disambig = undefined;
            regtoken.sortkeys = undefined;
            regtoken.ambig = undefined;

            Item = this.state.retrieveItem(key);
            //old_akey = akey;
            //akey = CSL.getAmbiguousCite.call(this.state, Item);
            //if (this.state.tmp.taintedItemIDs && this.state.opt.update_mode !== CSL.NUMERIC && old_akey !== akey) {
            //    print("Does this happen? If so: "+old_akey+" :: "+akey);
            //    this.state.tmp.taintedItemIDs[key] = true;
            //}
            var akey = regtoken.ambig;
            if ("undefined" === typeof akey) {
                akey = CSL.getAmbiguousCite.call(this.state, Item);
                this.state.tmp.taintedItemIDs[key] = true;
            }
            this.registry[key] = regtoken;

            abase = CSL.getAmbigConfig.call(this.state);
            this.registerAmbigToken(akey, key, abase);

            this.akeys[akey] = true;
            this.touched[key] = true;
        }
    }

};

/*
 * Main disambiguation  += -1 can everything for disambiguation be
 * crunched into this function?
 */
CSL.Registry.prototype.setdisambigs = function () {
    var akey, leftovers, key, pos, len, id;
    //
    // Okay, more changes.  Here is where we resolve all disambiguation
    // issues for cites touched by the update.  The this.ambigcites set is
    // based on the complete short form of citations, and is the basis on
    // which names are added and minimal adding of initials or given names
    // is performed.
    //

    //
    // we'll save a list of leftovers for each disambig pool.
    this.leftovers = [];
    //
    //  8.  Set disambiguation parameters on each inserted item token.
    //
    for (akey in this.akeys) {
        if (this.akeys.hasOwnProperty(akey)) {
            //
            // Disambiguation is fully encapsulated.
            // Disambiguator will run only if there are multiple
            // items, and at least one disambiguation mode is
            // in effect.
            this.state.disambiguate.run(akey);
        }
    }
    this.akeys = {};
};



CSL.Registry.prototype.renumber = function () {
    var len, pos, item;
    //
    // 19. Reset citation numbers on list items
    //
    if (this.state.opt.citation_number_sort_direction === CSL.DESCENDING
       && this.state.opt.citation_number_sort_used) {
        //this.reflist.reverse();
    }
    len = this.reflist.length;
    for (pos = 0; pos < len; pos += 1) {
        item = this.reflist[pos];
        // save the overhead of rerenderings if citation-number is not
        // used in the style.
        item.seq = (pos + 1);
        // update_mode is set to CSL.NUMERIC if citation-number is rendered
        // in citations.
        if (this.state.tmp.taintedItemIDs && item.seq != this.oldseq[item.id]) {
            if (this.state.opt.update_mode === CSL.NUMERIC) {
                this.state.tmp.taintedItemIDs[item.id] = true;
            }
            if (this.state.opt.bib_mode === CSL.NUMERIC) {
                this.return_data.bibchange = true;
            }
        }
    }
    if (this.state.opt.citation_number_sort_direction === CSL.DESCENDING
       && this.state.opt.citation_number_sort_used) {
        this.reflist.reverse();
    }
};

CSL.Registry.prototype.setsortkeys = function () {
    var key;
    //
    // 17. Set sort keys on each item token.
    //
    for (key in this.touched) {
        if (this.touched.hasOwnProperty(key)) {
            this.registry[key].sortkeys = CSL.getSortKeys.call(this.state, this.state.retrieveItem(key), "bibliography_sort");
            //CSL.debug("touched: "+item+" ... "+this.registry[item].sortkeys);
        }
    }
};

CSL.Registry.prototype.sorttokens = function () {
    //
    // 18. Resort token list.
    //
    this.reflist.sort(this.sorter.compareKeys);
};

/**
 * Compare two sort keys
 * <p>Nested, because keys are an array.</p>
 */
CSL.Registry.Comparifier = function (state, keyset) {
    var sort_directions, len, pos, compareKeys;
    var sortCompare = CSL.getSortCompare();
    sort_directions = state[keyset].opt.sort_directions;
    this.compareKeys = function (a, b) {
        len = a.sortkeys.length;
        for (pos = 0; pos < len; pos += 1) {
            //
            // for ascending sort 1 uses 1, -1 uses -1.
            // For descending sort, the values are reversed.
            //
            // Need to handle undefined values.  No way around it.
            // So have to screen .localeCompare (which is also
            // needed) from undefined values.  Everywhere, in all
            // compares.
            //
            var cmp = 0;
            if (a.sortkeys[pos] === b.sortkeys[pos]) {
                cmp = 0;
            } else if ("undefined" === typeof a.sortkeys[pos]) {
                cmp = sort_directions[pos][1];
            } else if ("undefined" === typeof b.sortkeys[pos]) {
                cmp = sort_directions[pos][0];
            } else {
                // cmp = a.sortkeys[pos].localeCompare(b.sortkeys[pos]);
                cmp = sortCompare(a.sortkeys[pos], b.sortkeys[pos]);
            }
            if (0 < cmp) {
                return sort_directions[pos][1];
            } else if (0 > cmp) {
                return sort_directions[pos][0];
            }
        }
        if (a.seq > b.seq) {
            return 1;
        } else if (a.seq < b.seq) {
            return -1;
        }
        return 0;
    };
    compareKeys = this.compareKeys;
    this.compareCompositeKeys = function (a, b) {
        return compareKeys(a[1], b[1]);
    };
};


/**
 * Compare two disambiguation tokens by their registry sort order
 * <p>Disambiguation lists need to be sorted this way, to
 * obtain the correct year-suffix when that's used.</p>
 */
CSL.Registry.prototype.compareRegistryTokens = function (a, b) {
    if (a.seq > b.seq) {
        return 1;
    } else if (a.seq < b.seq) {
        return -1;
    }
    return 0;
};

CSL.Registry.prototype.registerAmbigToken = function (akey, id, ambig_config) {
    //SNIP-START
    if (!this.registry[id]) {
        CSL.debug("Warning: unregistered item: itemID=("+id+"), akey=("+akey+")");
    }
    //SNIP-END
    // Taint if number of names to be included has changed
    // ZZZZZ Hullo, this might be the place to slot in cache invalidation
    // for cached disambiguation. That idea might have some life in it yet.
    if (this.registry[id] && this.registry[id].disambig && this.registry[id].disambig.names) {
        for (var i = 0, ilen = ambig_config.names.length; i < ilen; i += 1) {
            var new_names_params = ambig_config.names[i];
            var old_names_params = this.registry[id].disambig.names[i];
            if (new_names_params !== old_names_params) {
                this.state.tmp.taintedItemIDs[id] = true;
            }
        }
    }

    if (!this.ambigcites[akey]) {
        this.ambigcites[akey] = [];
    }
    if (this.ambigcites[akey].indexOf("" + id) === -1) {
        this.ambigcites[akey].push("" + id);
    }
    this.registry[id].ambig = akey;
    var dome = false;
    this.registry[id].disambig = CSL.cloneAmbigConfig(ambig_config);
};


/**
 * Get the sort key of an item, without decorations
 * <p>This is used internally by the Registry.</p>
 */
CSL.getSortKeys = function (Item, key_type) {
    var area, extension, strip_prepositions, use_parallels, len, pos;
    //SNIP-START
    if (false) {
        CSL.debug("KEY TYPE: " + key_type);
    }
    //SNIP-END
    area = this.tmp.area;
    extension = this.tmp.extension;
    strip_prepositions = CSL.Util.Sort.strip_prepositions;
    this.tmp.area = key_type;
    this.tmp.extension = "_sort";
    this.tmp.disambig_override = true;
    this.tmp.disambig_request = false;
    use_parallels = this.parallel.use_parallels;
    this.parallel.use_parallels = false;
    this.tmp.suppress_decorations = true;
    CSL.getCite.call(this, Item);
    this.tmp.suppress_decorations = false;
    this.parallel.use_parallels = use_parallels;
    this.tmp.disambig_override = false;
    len = this[key_type].keys.length;
    for (pos = 0; pos < len; pos += 1) {
        this[key_type].keys[pos] = strip_prepositions(this[key_type].keys[pos]);
    }
    //SNIP-START
    if (false) {
        CSL.debug("sort keys (" + key_type + "): " + this[key_type].keys);
    }
    //SNIP-END
    
    this.tmp.area = area;
    this.tmp.extension = extension;
    return this[key_type].keys;
};

