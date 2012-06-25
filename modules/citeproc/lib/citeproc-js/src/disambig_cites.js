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
    this.debug = false;
};

CSL.Disambiguation.prototype.run = function(akey) {
    if (!this.modes.length) {
        return;
    }
    //SNIP-START
    if (this.debug) {
        print("[A] === RUN ===");
    }
    //SNIP-END
    if (this.initVars(akey)) {
        this.runDisambig();
    }

};

CSL.Disambiguation.prototype.runDisambig = function () {
    var pos, len, ppos, llen, pppos, lllen, ismax;
    //SNIP-START
    if (this.debug) {
        print("[C] === runDisambig() ===");
    }
    //SNIP-END
    this.initGivens = true;
    //
    // Length of list may change during processing
    while (this.lists.length) {
        this.gnameset = 0;
        this.gname = 0;
        this.clashes = [1, 0];
        // each list is scanned repeatedly until all
        // items either succeed or ultimately fail.
        while(this.lists[0][1].length) {
            this.listpos = 0;
            if (!this.base) {
                this.base = this.lists[0][0];
            }
            var names_used = [];
            ismax = this.incrementDisambig();
            this.scanItems(this.lists[0]);
            this.evalScan(ismax);
        }
        this.lists = this.lists.slice(1);
    }
};

CSL.Disambiguation.prototype.scanItems = function (list) {
    var pos, len, Item, otherItem, ItemCite, ignore, base;
    //SNIP-START
    if (this.debug) {
        print("[2] === scanItems() ===");
    }
    //SNIP-END

    this.Item = list[1][0];
    this.ItemCite = CSL.getAmbiguousCite.call(this.state, this.Item, this.base);

    this.scanlist = list[1];
    this.partners = [];
    this.partners.push(this.Item);
    this.nonpartners = [];
    var clashes = 0;

    for (pos = 1, len = list[1].length; pos < len; pos += 1) {
        otherItem = list[1][pos];
        var otherItemCite = CSL.getAmbiguousCite.call(this.state, otherItem, this.base);
        //SNIP-START
        if (this.debug) {
            if (pos > 1) {
                print("  -----------");
            }
        }
        //SNIP-END
        if (this.ItemCite === otherItemCite) {
            //SNIP-START
            if (this.debug) {
                print("  [CLASH]--> "+this.Item.id+": "+this.ItemCite);
                print("             "+otherItem.id+": "+otherItemCite);
            }
            //SNIP-END
            clashes += 1;
            this.partners.push(otherItem);
        } else {
            //SNIP-START
            if (this.debug) {
                print("  [clear]--> "+this.Item.id+": "+this.ItemCite);
                print("             "+otherItem.id+": "+otherItemCite);
            }
            //SNIP-END
            this.nonpartners.push(otherItem);
        }
    }
    this.clashes[0] = this.clashes[1];
    this.clashes[1] = clashes;
};

CSL.Disambiguation.prototype.evalScan = function (maxed) {
    this[this.modes[this.modeindex]](maxed);
    if (maxed) {
        if (this.modeindex < this.modes.length - 1) {
            this.modeindex += 1;
        } else {
            this.lists[this.listpos + 1] = [this.base, []];
        }
    }
};

CSL.Disambiguation.prototype.disNames = function (ismax) {
    var pos, len, mybase, i, ilen;
    
    //SNIP-START
    if (this.debug) {
        print("[3] == disNames() ==");
        //print("       partners: "+[this.partners[i].id for (i in this.partners)].join(", "));
        //print("    nonpartners: "+[this.nonpartners[i].id for (i in this.nonpartners)].join(", "));
    }
    //SNIP-END

    // New design
    // this.base is a forward-only counter. Values are never
    // reduced, and the counter object is never overwritten.
    // It is methodically pushed forward in single-unit increments
    // in incrementDisambig() until disNames() wipes out the list.

    // this.betterbase is cloned from this.base exactly once,
    // at the start of a disambiguation run. Whenever an operation
    // results in improvement, the just-incremented elements
    // identified as this.base.names[this.gnameset] (number of
    // names)and as this.base.givens[this.gnameset][this.gname]
    // (level of given name) are copied from this.base.

    // The this.base object is used to control disambiguation
    // renderings. These will be more fully expanded than the final
    // text, but the flip side of the fact that the extra data does
    // not contribute anything to disambiguation is that leaving
    // it in does no harm -- think of it as the Cold Dark Matter of
    // disambiguation.

    if (this.clashes[1] === 0 && this.nonpartners.length === 1) {
        this.captureStepToBase();
        //SNIP-START
        if (this.debug) {
            print("  ** RESOLUTION [a]: lone partner, one nonpartner");
            print("  registering "+this.partners[0].id+" and "+this.nonpartners[0].id);
        }
        //SNIP-END
        this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, this.betterbase);
        this.state.registry.registerAmbigToken(this.akey, "" + this.partners[0].id, this.betterbase);
        this.lists[this.listpos] = [this.betterbase, []];
    } else if (this.clashes[1] === 0) {
        this.captureStepToBase();
        //SNIP-START
        if (this.debug) {
            print("  ** RESOLUTION [b]: lone partner, unknown number of remaining nonpartners");
            print("  registering "+this.partners[0].id);
        }
        //SNIP-END
        this.state.registry.registerAmbigToken(this.akey, "" + this.partners[0].id, this.betterbase);
        this.lists[this.listpos] = [this.betterbase, this.nonpartners];
        if (this.nonpartners.length) {
            this.initGivens = true;
        }
    } else if (this.nonpartners.length === 1) {
        this.captureStepToBase();
        //SNIP-START
        if (this.debug) {
            print("  ** RESOLUTION [c]: lone nonpartner, unknown number of partners remaining");
            print("  registering "+this.nonpartners[0].id);
        }
        //SNIP-END
        this.state.registry.registerAmbigToken(this.akey, "" + this.nonpartners[0].id, this.betterbase);
        //this.lists[this.listpos] = [this.betterbase, this.partners];
        this.lists[this.listpos] = [this.betterbase, this.partners];
    } else if (this.clashes[1] < this.clashes[0]) {
        this.captureStepToBase();
        //SNIP-START
        if (this.debug) {
            print("  ** RESOLUTION [d]: better result, but no entries safe to register");
        }
        //SNIP-END
        this.lists[this.listpos] = [this.betterbase, this.partners];
        this.lists.push([this.betterbase, this.nonpartners]);
    } else {
        //SNIP-START
        if (this.debug) {
            print("  ** RESOLUTION [e]: no improvement, and clashes remain");
        }
        //SNIP-END
        if (ismax) {
            this.lists[this.listpos] = [this.betterbase, this.nonpartners];
            this.lists.push([this.betterbase, this.partners]);
            if (this.modeindex === this.modes.length - 1) {
                //SNIP-START
                if (this.debug) {
                    print("     (registering clashing entries because we've run out of options)");
                }
                //SNIP-END
                for (i = 0, ilen = this.partners.length; i < ilen; i += 1) {
                    this.state.registry.registerAmbigToken(this.akey, "" + this.partners[i].id, this.betterbase);
                }
                this.lists[this.listpos] = [this.betterbase, []];
            }
        }
    }
};

CSL.Disambiguation.prototype.disExtraText = function () {
    var pos, len, mybase;
    
    //SNIP-START
    if (this.debug) {
        print("[3] === disExtraText ==");
    }
    //SNIP-END

    // If first encounter in this cycle and multiple modes are
    // available, decrement mode and reset base
    if (this.modes.length > 1 && !this.base.disambiguate) {
        this.modeindex = 0;
        this.base = CSL.cloneAmbigConfig(this.betterbase);
    }
    // If disambiguate is false set to true
    if (!this.betterbase.disambiguate) {
        this.base.disambiguate = true;
        this.betterbase.disambiguate = true;
        this.initGivens = true;
    } else {
        if (this.modeindex === this.modes.length - 1) {
            // Give up at the second try if 
            // If we reach this, we will not have disambiguated fully.
            var base = this.lists[this.listpos][0];
            for (var i = 0, ilen = this.lists[this.listpos][1].length; i < ilen; i += 1) {
                this.state.registry.registerAmbigToken(this.akey, "" + this.lists[this.listpos][1][i].id, base);
            }
            this.lists[this.listpos] = [this.betterbase, []];
        }
    }
};

CSL.Disambiguation.prototype.disYears = function () {
    var pos, len, tokens, token, item;
    //SNIP-START
    if (this.debug) {
        print("[3] === disYears ==");
    }
    //SNIP-END
    tokens = [];
    var base = this.lists[this.listpos][0];
    if (this.clashes[1]) {
        // That is, if the initial increment on the ambigs group returns no
        // clashes, don't apply suffix. The condition is a necessary failsafe.
		// In original submission order
		for (var i = 0, ilen = this.state.registry.mylist.length; i < ilen; i += 1) {
			var origid = this.state.registry.mylist[i];
			for (var j = 0, jlen = this.lists[this.listpos][1].length; j < jlen; j += 1) {
				var token = this.lists[this.listpos][1][j];
				// Warning: token.id can be number. This should be fixed at a higher level in citeproc-js if poss.
				if (token.id == origid) {
					tokens.push(this.registry[token.id]);
					break;
				}
			}
		}
    }
    tokens.sort(this.state.registry.sorter.compareKeys);
    for (pos = 0, len = tokens.length; pos < len; pos += 1) {
        base.year_suffix = ""+pos;
        var oldBase = this.state.registry.registry[tokens[pos].id].disambig;
        this.state.registry.registerAmbigToken(this.akey, "" + tokens[pos].id, base);
        if (CSL.ambigConfigDiff(oldBase,base)) {
            this.state.tmp.taintedItemIDs[tokens[pos].id] = true;
        }
    }
    this.lists[this.listpos] = [this.betterbase, []];
};

CSL.Disambiguation.prototype.incrementDisambig = function () {
    var val;
    //SNIP-START
    if (this.debug) {
        print("\n[1] === incrementDisambig() ===");
    }
    //SNIP-END
    if (this.initGivens) {
        this.initGivens = false;
        return false;
    }
    var maxed = false;
    var increment_names = true;
    var increment_givens = true;
    if ("disNames" === this.modes[this.modeindex]) {
        // this.gnameset: the index pos of the current nameset
        // this.gname: the index pos of the current name w/in the current nameset
        
        // Stages:
        // - Increment givenname (optional)
        // - Add a name (optional)
        // - Move to next nameset

        // Incrementing is done forward-only on this.base. Values
        // that improve disambiguation results are copied to
        // this.betterbase, which is used to set the disambig parameters
        // in the processor registry.
        
        // Increment
        // Max val is always true if a level is inactive.
        increment_names = false;
        if ("number" !== typeof this.givensMax) {
            increment_names = true;
        }
        var increment_namesets = false;
        if ("number" !== typeof this.namesMax) {
            increment_namesets = true;
        }
        if ("number" === typeof this.givensMax) {
            if (this.base.givens[this.gnameset][this.gname] < this.givensMax) {
                this.base.givens[this.gnameset][this.gname] += 1;
            } else {
                increment_names = true;
            }
        }
        if ("number" === typeof this.namesMax && increment_names) {
            increment_namesets = false;
            if (this.base.names[this.gnameset] < this.namesMax) {
                this.base.names[this.gnameset] += 1;
                this.gname += 1;
            } else {
                increment_namesets = true;
            }
        }
        if ("number" === typeof this.namesetsMax && increment_namesets) {
            if (this.gnameset < this.namesetsMax) {
                this.gnameset += 1;
                this.base.names[this.gnameset] = 1;
                this.gname = 0;
            } else {
                var increment_mode = true;
            }
        }
        //SNIP-START
        if (this.debug) {
            print("    ------------------");
            print("    incremented values");
            print("    ------------------");
            print("    | gnameset: "+this.gnameset);
            print("    | gname: "+this.gname);
            print("    | names value: "+this.base.names[this.gnameset]);
            if (this.base.givens.length) {
                print("    | givens value: "+this.base.givens[this.gnameset][this.gname]);
            } else {
                print("    | givens value: nil");
            }
            print("    | namesetsMax: "+this.namesetsMax);
            print("    | namesMax: "+this.namesMax);
            print("    | givensMax: "+this.givensMax);
        }
        //SNIP-END
        if (("number" !== typeof this.namesetsMax || this.namesetsMax === -1 || this.gnameset === this.namesetsMax)
            && ("number" !== typeof this.namesMax || this.base.names[this.gnameset] === this.namesMax)
            && ("number" != typeof this.givensMax || "undefined" === typeof this.base.givens[this.gnameset][this.gname] || this.base.givens[this.gnameset][this.gname] === this.givensMax)) {
  

            maxed = true;
            //SNIP-START
            if (this.debug) {
                print("    MAXED");
            }
            //SNIP-END
        }
    }
    if ("disYears" === this.modes[this.modeindex]) {
    }
    return maxed;
};

CSL.Disambiguation.prototype.initVars = function (akey) {
    var i, ilen, myIds, myItemBundles, myItems;
    //SNIP-START
    if (this.debug) {
        print("[B] === initVars() ===");
    }
    //SNIP-END
    this.lists = [];
    this.base = false;
    this.betterbase = false;
    this.akey = akey;

    this.maxNamesByItemId = {};


    myItemBundles = [];
    myIds = this.ambigcites[akey];
    if (!myIds || !myIds.length) {
        return false;
    }
    var Item = false;
    var myItem = this.state.retrieveItem("" + myIds[0]);
    this.getCiteData(myItem);
    this.base = CSL.getAmbigConfig.call(this.state);
    if (myIds && myIds.length > 1) {
        myItemBundles.push([this.maxNamesByItemId[myItem.id], myItem]);
        // Build a composite list of Items and associated
        // max names. This is messy, but it's the only
        // way to get the items sorted by the number of names
        // to be disambiguated. If they are in descending order
        // with name expansions, the processor will hang.
        for (i = 1, ilen = myIds.length; i < ilen; i += 1) {
            myItem = this.state.retrieveItem("" + myIds[i]);
            this.getCiteData(myItem, this.base);
            myItemBundles.push([this.maxNamesByItemId[myItem.id], myItem]);
        }
        myItemBundles.sort(
            function (a, b) {
                if (a[0] > b[0]) {
                    return 1;
                } else if (a[0] < b[0]) {
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
        this.lists.push([this.base, myItems]);
        this.Item = this.lists[0][1][0];
    } else {
        this.Item = this.state.retrieveItem("" + myIds[0]);
    }

    this.modeindex = 0;
    this.namesMax = this.maxNamesByItemId[this.Item.id][0];

    for (i = 0, ilen = this.base.givens.length; i < ilen; i += 1) {
        for (var j = 0, jlen = this.namesMax; j < jlen; j += 1) {
            if (!this.base.givens[i][j]) {
                this.base.givens[i][j] = 0;
                this.betterbase.givens[i][j] = 0;
            }
        }
    }

    this.base.year_suffix = false;
    this.base.disambiguate = false;
    this.betterbase.year_suffix = false;
    this.betterbase.disambiguate = false;
    if (this.state.opt["givenname-disambiguation-rule"] === "by-cite") {
        this.givensMax = 2;
    }
    return true;
};

/**
 * Set available modes for disambiguation
 */
CSL.Disambiguation.prototype.configModes = function () {
    var dagopt, gdropt;
    // Modes are function names prototyped to this instance.
    this.modes = [];
    dagopt = this.state.opt["disambiguate-add-givenname"];
    gdropt = this.state.opt["givenname-disambiguation-rule"];
    if (this.state.opt['disambiguate-add-names'] || (dagopt && gdropt === "by-cite")) {
        this.modes.push("disNames");
    }
    if (this.state.opt.has_disambiguate) {
        this.modes.push("disExtraText");
    }
    if (this.state.opt["disambiguate-add-year-suffix"]) {
        this.modes.push("disYears");
    }
};

CSL.Disambiguation.prototype.getCiteData = function(Item, base) {
    // Initialize base if first set item seen
    if (!this.maxNamesByItemId[Item.id]) {
        CSL.getAmbiguousCite.call(this.state, Item, base);
        base = CSL.getAmbigConfig.call(this.state);
        this.maxNamesByItemId[Item.id] = CSL.getMaxVals.call(this.state);
        this.state.registry.registry[Item.id].disambig.givens = this.state.tmp.disambig_settings.givens.slice();
        this.namesetsMax = this.state.registry.registry[Item.id].disambig.names.length - 1;
        if (!this.base) {
            this.base = base;
            this.betterbase = CSL.cloneAmbigConfig(base);
        }
        if (base.names.length < this.base.names.length) {
            // I don't know what would happen with discrepancies in the number
            // of namesets rendered on items, so we use the fewer of the two
            // and limit the other to that size.
            this.base = base;
        }
        // Padding. Within namesets, we use the longer of the two throughout.
        var update = false;
        for (var i = 0, ilen = base.names.length; i < ilen; i += 1) {
            if (base.names[i] > this.base.names[i]) {
                this.base.givens[i] = this.base.givens[i].concat(this.base.givens[i].slice(this.base.names[i]));
                this.base.names[i] = base.names[i];
                this.betterbase.names = this.base.names.slice();
            }
        }
        // This shouldn't be necessary
        // getAmbiguousCite() should return a valid and complete
        // givens segment under all conditions, but it does not
        // do so for institution authors, so we clean up after it
        // here.
        // Relevant test: sort_ChicagoYearSuffix2
        this.betterbase.givens = this.base.givens.slice();
        for (var j = 0, jlen = this.base.givens.length; j < jlen; j += 1) {
            this.betterbase.givens[j] = this.base.givens[j].slice();
        }
    }
};

CSL.Disambiguation.prototype.captureStepToBase = function() {
    if (this.state.opt["givenname-disambiguation-rule"] === "by-cite") {
        this.betterbase.givens[this.gnameset][this.gname] = this.base.givens[this.gnameset][this.gname];
    }
    this.betterbase.names[this.gnameset] = this.base.names[this.gnameset];
};
