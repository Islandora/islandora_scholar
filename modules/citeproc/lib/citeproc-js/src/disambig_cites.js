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

CSL.Disambiguation = function (state) {
    this.state = state;
    this.sys = this.state.sys;
    this.registry = state.registry.registry;
    this.ambigcites = state.registry.ambigcites;
    this.configModes();
    this.clashes = [1, 0];
};

CSL.Disambiguation.prototype.run = function(akey) {
    if (!this.modes.length) {
        return;
    }
    // print("== RUN ==");
    this.initVars(akey);
    this.runDisambig();
};

CSL.Disambiguation.prototype.runDisambig = function () {
    var pos, len, ppos, llen, pppos, lllen, ismax;
    //print("=== runDisambig() ===");
    //
    // length is evaluated inside the loop condition by intention
    // here; items will be added to the list during processing.
    for (pos = 0; pos < this.lists.length; pos += 1) {
        this.nnameset = 0;
        this.gnameset = 0;
        this.gname = 0;
        // each list is scanned repeatedly until all
        // items either succeed or ultimately fail.
        while(this.lists[pos][1].length) {
            this.listpos = pos;
            if (!this.base) {
                this.base = this.lists[pos][0];
            }
            if (this.rerun) {
                this.rerun = false;
            } else {
                this.scanItems(this.lists[pos], 0);
            }
            var names_used = [];
            ismax = this.incrementDisambig();
            this.scanItems(this.lists[pos], 1);

            // Add to scanItems()
            if (this.clashes[1] < this.clashes[0]) {
                //print("Clashes reduced to: "+this.clashes[1]);
                //print("  setting \"better\" to: ["+this.base.names+"]");
                this.base.better = this.base.names.slice();
            }

            this.evalScan(ismax);
        }
    }
};

CSL.Disambiguation.prototype.scanItems = function (list, phase) {
    var pos, len, Item, otherItem, ItemCite, ignore, base;
    //print("=== scanItems() ===");
    Item = list[1][0];
    this.scanlist = list[1];
    this.partners = [];

    var tempResult = this.getItemDesc(Item);
    this.base = tempResult[0];
    if (!phase) {
        this.base.disambiguate = false;
    }
    this.maxvals = tempResult[1];
    this.minval = tempResult[2];
    ItemCite = tempResult[3];

    this.partners.push(Item);
    this.clashes[phase] = 0;
    this.nonpartners = [];
    for (pos = 1, len = list[1].length; pos < len; pos += 1) {
        otherItem = list[1][pos];
        var otherItemData = this.getItemDesc(otherItem);
        var otherItemCite = otherItemData[3];
        //otherItemBase = otherItemData[0];

        // FIXED
        // print("  --> "+Item.id+": ("+ItemCite+") "+otherItem.id+": ("+otherItemCite+")");
        if (ItemCite === otherItemCite) {
            // print("    clash");
            this.clashes[phase] += 1;
            this.partners.push(otherItem);
        } else {
            // print("    non-clash");
            this.nonpartners.push(otherItem);
        }
    }
};

CSL.Disambiguation.prototype.evalScan = function (ismax) {
    // print("MODE: "+this.modeindex+" "+this.modes);
    // FIXED
    //print("Mode: "+this.modes[this.modeindex]+" names: "+this.base.names[this.nnameset]);
    this[this.modes[this.modeindex]](ismax);
};

CSL.Disambiguation.prototype.disNames = function (ismax) {
    var pos, len, mybase;
    //print("== disNames ==")
    if (this.clashes[1] === 0 && this.nonpartners.length === 1) {
        mybase = CSL.cloneAmbigConfig(this.base);
        mybase.year_suffix = false;
        this.state.registry.registerAmbigToken(this.akey, "" + this.partners[0].id, mybase);
        this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, mybase);
        this.lists[this.listpos] = [this.base, []];
    } else if (this.clashes[1] === 0) {
        // no clashes
        // Remove item from list.  If only one non-clashing item,
        // remove it as well.
        
        // Note that this.partners has length of exactly one in this case,
        // because there are no clashes. As we've therefore resolved conflicts
        // by adding names, we clone the base and quash any lurking year_suffix
        // value.
        mybase = CSL.cloneAmbigConfig(this.base);
        mybase.year_suffix = false;
        this.state.registry.registerAmbigToken(this.akey, "" + this.partners[0].id, mybase);
        this.lists[this.listpos] = [this.base, []];
    } else if (this.nonpartners.length === 1) {
        mybase = CSL.cloneAmbigConfig(this.base);
        mybase.year_suffix = false;
        this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, mybase);
        this.lists[this.listpos] = [this.base, this.partners];
    } else if (this.clashes[1] < this.clashes[0]) {
        // fewer clashes
        // requeue nonpartners, and remove them from the list
        this.lists[this.listpos] = [this.base, this.partners];
        if (this.nonpartners.length === 1) {
            // this.state.registry.registerAmbigToken(this.akey, this.nonpartners[0].id, this.base, this.scanlist);
            this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, this.base);
        } else {
            this.lists.push([this.base, this.nonpartners]);
        }
    } else {
        if (ismax || this.advance_mode) {
            if (ismax) {
                var better = this.lists[this.listpos][0].better;
                //print("-- ejecting in disNames() @ " + better);
                if (better) {
                    this.base.names = better.slice();
                } else {
                    this.base = new CSL.AmbigConfig();
                }
                this.lists[this.listpos] = [this.base, this.nonpartners];
            }
            for (pos = 0, len = this.partners.length; pos < len; pos += 1) {
                this.state.registry.registerAmbigToken(this.akey, "" + this.partners[pos].id, this.base);
            }
        } else {
            // Disambiguation actually runs very slightly faster
            // when this and the similar toggle in disGivens()
            // are used. Go figure.
            this.rerun = true;
        }
    }
    //print("== disNames (end) ==")
};


CSL.Disambiguation.prototype.disGivens = function (ismax) {
    var pos, len, mybase;
    //print("== disGivens ==")
    if (this.clashes[1] === 0 && this.nonpartners.length === 1) {
        if (this.clashes[0] === 1) {
            this.base = this.decrementNames();
        }
        mybase = CSL.cloneAmbigConfig(this.base);
        mybase.year_suffix = false;
        this.state.registry.registerAmbigToken(this.akey, "" + this.partners[0].id, this.base);
        this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, mybase);
        
        this.lists[this.listpos] = [this.base, []];
    } else if (this.clashes[1] === 0) {
        if (this.clashes[0] === 1) {
            this.base = this.decrementNames();
        }
        mybase = CSL.cloneAmbigConfig(this.base);
        mybase.year_suffix = false;
        this.state.registry.registerAmbigToken(this.akey, "" + this.partners[0].id, mybase);
        this.lists[this.listpos] = [this.base, this.nonpartners];
    } else if (this.nonpartners.length === 1) {
        if (this.clashes[0] === 1) {
            this.base = this.decrementNames();
        }
        mybase = CSL.cloneAmbigConfig(this.base);
        mybase.year_suffix = false;
        this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, mybase);
        this.lists[this.listpos] = [this.base, this.partners];
    } else if (this.clashes[1] < this.clashes[0]) {
        // fewer clashes
        // requeue nonpartners, and remove them from the list
        this.lists[this.listpos] = [this.base, this.partners];
        if (this.nonpartners.length === 1) {
            // this.state.registry.registerAmbigToken(this.akey, this.nonpartners[0].id, this.base, this.scanlist);
            this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, this.base);
        } else {
            this.lists.push([this.base, this.nonpartners]);
        }
    } else {
        this.base = CSL.cloneAmbigConfig(this.oldbase);
        if (ismax || this.advance_mode) {
            if (ismax) {
                var better = this.lists[this.listpos][0].better;
                //print("-- ejecting in disGivens() @ "+ better);
                if (better) {
                    this.base.names = better.slice();
                } else {
                    this.base = new CSL.AmbigConfig();
                }
                this.lists[this.listpos] = [this.base, this.nonpartners];
            }
            for (pos = 0, len = this.partners.length; pos < len; pos += 1) {
                this.state.registry.registerAmbigToken(this.akey, "" + this.partners[pos].id, this.base);
            }
        } else {
            this.rerun = true;
        }
    }
};


CSL.Disambiguation.prototype.disExtraText = function () {
    var pos, len, mybase;
    // Try with disambiguate="true""
    //print("=== disExtraText ==");
    if (this.clashes[1] === 0) {
        // See note in disNames, above.
        mybase = CSL.cloneAmbigConfig(this.base);
        mybase.year_suffix = false;
        this.state.registry.registerAmbigToken(this.akey, "" + this.partners[0].id, mybase);
        if (this.nonpartners.length === 1) {
            this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, mybase);
            this.lists[this.listpos] = [this.base,[]];
        } else {
            this.lists[this.listpos] = [this.base, this.nonpartners];
        }
    } else {
        this.base.disambiguate = false;
        this.lists[this.listpos] = [this.base, this.lists[this.listpos][1].slice(1)];
    }
};

CSL.Disambiguation.prototype.disYears = function () {
    var pos, len, tokens, token, item;
    //print("=== disYears ==");
    tokens = [];
    //print("=====");
    //print("IDs: " + [this.lists[this.listpos][1][x].id for (x in this.lists[this.listpos][1])]);
    //print("=====");
    for (pos = 0, len = this.lists[this.listpos][1].length; pos < len; pos += 1) {
        token = this.registry[this.lists[this.listpos][1][pos].id];
        tokens.push(token);
    }
    tokens.sort(this.state.registry.sorter.compareKeys);
    for (pos = 0, len = tokens.length; pos < len; pos += 1) {
        // Only pass this.scanlist on the first iteration.  The
        // list will be iterated on execution, and should only
        // be run once, to avoid losing update markers.
        //print("  ??? [" +pos+ "] " +oldys);
        if (pos === 0) {
            //this.state.registry.registerAmbigToken(this.akey, "" + tokens[pos].id, this.base, this.scanlist);
            this.base.year_suffix = ""+pos;
            this.state.registry.registerAmbigToken(this.akey, "" + tokens[pos].id, this.base);
        } else {
            this.base.year_suffix = ""+pos;
            this.state.registry.registerAmbigToken(this.akey, "" + tokens[pos].id, this.base);
        }
        //this.state.registry.registry[tokens[pos].id].disambig.year_suffix = ""+pos;
        //tokens[pos].disambig.year_suffix = ""+pos;
        //var newys = tokens[pos].disambig.year_suffix;
        var newys = this.state.registry.registry[tokens[pos].id].disambig.year_suffix;
        //print("     --> [" +pos+ "] " +newys);
        if (this.old_desc[tokens[pos].id][0] !== newys) {
            this.state.tmp.taintedItemIDs[tokens[pos].id] = true;
        }
    }
    this.lists[this.listpos] = [this.base, []];
    //print("=== disYears (end) ==");
};

CSL.Disambiguation.prototype.incrementDisambig = function () {
    var val, maxed;
    //print("=== incrementDisambig() ===");
    maxed = false;
    this.oldbase = CSL.cloneAmbigConfig(this.base);
    if (this.advance_mode) {
        this.modeindex += 1;
        this.advance_mode = false;
    }
    if (!maxed && "disNames" === this.modes[this.modeindex]) {
        if (this.base.names[this.nnameset] < this.maxvals[this.nnameset]) {
            this.base.names[this.nnameset] += 1;
        } else {
            if (this.nnameset < (this.base.names.length - 1)) {
                this.nnameset += 1;
            }
            if (this.base.names[this.nnameset] < this.maxvals[this.nnameset]) {
                this.base.names[this.nnameset] += 1;
            }
        }
        if (this.nnameset === (this.base.names.length - 1) && this.base.names[this.nnameset] === this.maxvals[this.nnameset]) {
            if (this.modeindex === (this.modes.length - 1)) {
                return true;
            } else {
                maxed = false;
            }
        }
    }
    if (!maxed && "disGivens" === this.modes[this.modeindex]) {
        if (this.gname < this.maxvals[this.gnameset]) {
            if (this.base.givens[this.gnameset][this.gname] === this.minval) {
                this.base.givens[this.gnameset][this.gname] += 1;
            }
            this.base.givens[this.gnameset][this.gname] += 1;
            this.gname += 1;
        } else {
            if (this.gnameset < (this.base.givens.length - 1)) {
                this.gnameset += 1;
                this.gname = 0;
            }
            if (this.gname < this.maxvals[this.gnameset]) {
                this.base.givens[this.gnameset][this.gname] += 1;
                this.gname += 1;
            }
        }
    }
    if (!maxed && "disExtraText" === this.modes[this.modeindex]) {
        maxed = false;
        this.base.disambiguate = true;
        if (this.modeindex === (this.modes.length - 1)) {
            return true;
        } else {
            maxed = false;
        }
    }
    if (!maxed && "disYears" === this.modes[this.modeindex]) {
        maxed = false;
    }
    if (!maxed && this.modes[this.modeindex] === "disGivens") {
        //print("  max check data ==> "+this.gname+" "+this.maxvals[this.gnameset]+" "+this.gnameset+" "+this.base.names.length);
        // Test for undefined in an inelegant safety catch to prevent overruns.
        // Sometime when I or someone else is feeling ambitious, maybe the cause
        // of the overrun currently seen in disambiguate_AndreaEg4 can be tracked
        // down.
        if ((this.gnameset >= (this.base.names.length - 1) && ("undefined" === typeof this.maxvals[this.gnameset] || this.gname === this.maxvals[this.gnameset])) || this.base.names.length === 0) {
            if (this.modeindex === (this.modes.length - 1)) {
                //print("TOTAL MAX disGivens");
                maxed = true;
            } else {
                this.advance_mode = true;
            }
        }
    }
    if (!maxed && this.modes[this.modeindex] === "disNames") {
        // Test for undefined is a safety catch. See note above on disGivens block.
        if ((this.nnameset >= (this.base.names.length - 1) && ("undefined" === typeof this.maxvals[this.nnameset] ||this.base.names[this.nnameset] === this.maxvals[this.nnameset])) || this.base.names.length === 0) {
            if (this.modeindex === (this.modes.length - 1)) {
                // print("TOTAL MAX disNames");
                maxed = true;
            } else {
                this.advance_mode = true;
            }
        }
    }
    return maxed;
};

CSL.Disambiguation.prototype.getItemDesc = function (Item, forceMax) {
    var str, maxvals, minval, base;
    //print("=== getItemDesc() ===");
    //print("getItem with name limits: "+this.base.names);
    str = CSL.getAmbiguousCite.call(this.state, Item, this.base);
    maxvals = CSL.getMaxVals.call(this.state);
    minval = CSL.getMinVal.call(this.state);
    base = CSL.getAmbigConfig.call(this.state);
    return [base, maxvals, minval, str];
};

CSL.Disambiguation.prototype.initVars = function (akey) {
    var i, ilen, myIds, myItemBundles, myItems;
    //print("=== initVars() ===");
    this.lists = [];
    this.base = false;
    this.akey = akey;
    this.advance_mode = false;
    myItemBundles = [];
    this.old_desc = {};
    myIds = this.ambigcites[akey];
    if (myIds && myIds.length > 1) {
        // Build a composite list of Items and associated
        // disambig objects. This is messy, but it's the only
        // way to get the items sorted by the number of names
        // to be disambiguated. If they are in descending order
        // with name expansions, the processor will hang.
        for (i = 0, ilen = myIds.length; i < ilen; i += 1) {
            var myItem = this.state.retrieveItem("" + myIds[i]);
            var myDesc = this.getItemDesc(myItem);
            myItemBundles.push([myDesc, myItem]);
            this.old_desc[myIds[i]] = [this.state.registry.registry[myIds[i]].disambig.year_suffix, this.state.registry.registry[myIds[i]].disambig.disambiguate];
        }
        myItemBundles.sort(
            function (a, b) {
                if (a[0][1] > b[0][1]) {
                    return 1;
                } else if (a[0][1] < b[0][1]) {
                    return -1;
                } else {
                    if (a[1].id > b[1].id) {
                        return 1;
                    } else if (a[1].id < b[1].id) {
                        return -1;
                    } else {
                        return 0;
                    }
                }
            }
        );
        myItems = [];
        for (i = 0, ilen = myItemBundles.length; i < ilen; i += 1) {
            myItems.push(myItemBundles[i][1]);
        }
        // FIXED
        //print("Item sequence: "+[myItems[x].id for (x in myItems)]);
        
        // first element is the base disambig, which is false for the initial
        // list.
        this.lists.push([this.base, myItems]);
    }
    this.modeindex = 0;
};

/**
 * Set available modes for disambiguation
 */
CSL.Disambiguation.prototype.configModes = function () {
    var dagopt, gdropt;
    // Modes are function names prototyped to this instance.
    this.modes = [];
    if (this.state.opt["disambiguate-add-names"]) {
        this.modes.push("disNames");
    }
    dagopt = this.state.opt["disambiguate-add-givenname"];
    gdropt = this.state.opt["givenname-disambiguation-rule"];
    if (dagopt && gdropt === "by-cite") {
        this.modes.push("disGivens");
    }
    if (this.state.opt.has_disambiguate) {
        this.modes.push("disExtraText");
    }
    if (this.state.opt["disambiguate-add-year-suffix"]) {
        this.modes.push("disYears");
    }
};

CSL.Disambiguation.prototype.decrementNames = function () {
    var base_return, do_me, i, j, pos, len, ppos, llen, ids;
    // This band-aid is needed for disGivens, to prevent name
    // overruns when an initial or givenname is belatedly
    // found to be sufficient for disambiguation.
    //
    // Two reverse scans, one to determine if there are any expanded
    // names to stop the unwind, and another to perform the
    // unwind
    base_return = CSL.cloneAmbigConfig(this.base);
    do_me = false;
    len = base_return.givens.length - 1;
    for (pos = len; pos > -1; pos += -1) {
        llen = base_return.givens[pos].length - 1;
        for (ppos = llen; ppos > -1; ppos += -1) {
            if (base_return.givens[pos][ppos] > this.oldbase.givens[pos][ppos]) {
                do_me = true;
            }
        }
    }
    if (do_me) {
        len = base_return.givens.length - 1;
        for (pos = len; pos > -1; pos += -1) {
            llen = base_return.givens[pos].length - 1;
            for (ppos = llen; ppos > -1; ppos += -1) {
                // FIXED
                if (base_return.givens[pos][ppos] > this.oldbase.givens[pos][ppos]) {
                    break;
                }
                // Be careful to treat the givens and names
                // arrays in step.  Fixes bug affecting
                // disambiguate_AllNamesBaseNameCountOnFailureIfYearSuffixAvailable
                if (ppos < base_return.names[pos]) {
                    base_return.names[pos] += -1;
                }
            }
        }
    }
    return base_return;
};
