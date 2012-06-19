/*
 * Copyright (c) Frank G. Bennett, Jr. 2009. All Rights Reserved.
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
 * Copyright (c) Frank G. Bennett, Jr. 2009. All Rights Reserved.
 */

/**
 * Determines the position of a citation.
 * Method of this class are called from registry.
 */
CSL.Util.Positioner = function(){};

CSL.Util.Positioner.prototype.getPosition = function(citation, update) {
	for(var previousIndex = citation.properties.index-1;
        previousIndex != -1
	    && (!this.citationsByIndex[previousIndex]
		|| this.citationsByIndex[previousIndex].properties["delete"]);
		previousIndex--) {}
    var previousCitation = (previousIndex == -1 ? false : this.citationsByIndex[previousIndex]);
    // if one source is the same as the last, use ibid
    //
    // Case 1: source in previous citation
    // (1) Threshold conditions
    //     (a) there must be a previous citation with one item
    //     (b) this item must be the first in this citation
    //     (c) the previous citation must contain a reference to the same item ...
    // Case 2: immediately preceding source in this citation
    // (1) Threshold conditions
    //     (a) there must be an imediately preceding reference to  the
    //         same item in this citation
    // Evaluation
    //     (a) if the previous citation had no locator and this citation
    //         has one, use ibid+pages
    //     (b) if the previous citation had no locator and this citation
    //         also has none, use ibid
    //     (c) if the previous citation had a locator (page number, etc.)
    //         and this citation has a locator that is identical, use ibid
    //     (d) if the previous citation had a locator, and this citation
    //         has one that differs, use ibid+pages
    //     (e) if the previous citation had a locator and this citation
    //         has none, use subsequent
    //     (f) if the current and previous citations are not the same,
    //         set as FIRST or SUBSEQUENT, as appropriate.
    //
    // For Case 1
    var curr = citation.citationItems[0];
	curr.thisRef = citation.properties.index+1;
	// pump priming
	if (!previousCitation) {
		curr.itemNumber = 1;
		this.firstItemNumber[curr.itemID] = 1;
	} else {
		curr.itemNumber = previousCitation.citationItems[previousCitation.citationItems.length-1].itemNumber+1;
	}
    if(previousCitation
       && previousCitation.citationItems.length == 1
       && citation.citationItems[0].itemID == previousCitation.citationItems[0].itemID ) {
		var newPosition = this.checkIbidOrSubsequent(previousCitation.citationItems[0], curr, citation);
    } else {
		// check outside this note if necessary
		var newPosition = this.checkFirstOrSubsequent(curr, citation );
    }
    this.updateCitePosition (curr, newPosition, citation, update);
    citation.citationItems[0].position = newPosition;
    this.checkCiteContext (citation.citationItems[0], newPosition);
    //
    // For Case 2
	// step through remaining cites in this citation
    var l = citation.citationItems.length;
	for ( var i = 1; i < l; i++) {
		// step through possible preceding cites within same note, from back to front
		var curr = citation.citationItems[i];
		var prev = citation.citationItems[i-1];
		curr.itemNumber = prev.itemNumber+1;
		curr.thisRef = citation.properties.index+1;
		var newPosition = undefined;
		var prev = citation.citationItems[i-1];
		if ( curr.itemID == prev.itemID ) {
			// check immediately preceding cite in this note
			newPosition = this.checkIbidOrSubsequent(prev, curr, citation);
		} else {
			// check other preceding cites
			newPosition = this.checkFirstOrSubsequent(curr, citation );
		}
		this.updateCitePosition (curr, newPosition, citation, true);
		citation.citationItems[i].position = newPosition;
		this.checkCiteContext (citation.citationItems[i], newPosition);
	}
}

/*
 * evaluates position of qualifying cites
 */

CSL.Util.Positioner.prototype.checkIbidOrSubsequent = function(prev, curr, citation) {
    var newPosition;

	curr.distanceToLastRef = citation.properties.index+1 - this.lastRefUnit[curr.itemID];
	curr.firstRefUnit = this.firstRefUnit[curr.itemID];
	this.lastRefUnit[curr.itemID] = citation.properties.index+1;
    if (!prev.locator) {
		if (curr.locator) {
			newPosition = Zotero.CSL.POSITION_IBID_WITH_LOCATOR;
		} else {
			newPosition = Zotero.CSL.POSITION_IBID;
		}
    } else {
		if (prev.locator == curr.locator) {
			newPosition = Zotero.CSL.POSITION_IBID;
		} else if (curr.locator) {
			newPosition = Zotero.CSL.POSITION_IBID_WITH_LOCATOR;
		} else {
			newPosition = Zotero.CSL.POSITION_SUBSEQUENT;
		}
    }
    return newPosition;
}

/*
 * checks whether a cite is a first or subsequent reference
 */
CSL.Util.Positioner.prototype.checkFirstOrSubsequent = function(curr, citation ) {
    var newPosition;

    if (!this.firstItemNumber[curr.itemID] || curr.itemNumber == this.firstItemNumber[curr.itemID]) {
		this.firstItemNumber[curr.itemID] = curr.itemNumber;
		curr.distanceToLastRef = 0;
		curr.firstRefUnit = citation.properties.index+1;
		this.firstRefUnit[curr.itemID] = citation.properties.index+1;
		this.lastRefUnit[curr.itemID] = citation.properties.index+1;
		newPosition = Zotero.CSL.POSITION_FIRST;
    } else {
		curr.distanceToLastRef = citation.properties.index+1 - this.lastRefUnit[curr.itemID];
		curr.firstRefUnit = this.firstRefUnit[curr.itemID];
		this.lastRefUnit[curr.itemID] = citation.properties.index+1;
		newPosition = Zotero.CSL.POSITION_SUBSEQUENT;
    }
    return newPosition;
}
