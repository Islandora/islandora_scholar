/*
 * Attempt to find copy of Item serialization in cache.
 *
 * If found, check match with received Item.

 *   If matched, attempt to find cached serialization
 *   of disambig under this Item ID.
 *
 *     If found, return the copy.

 *     If not found, open an entry mapping of this
 *     disambig serialization under the Item ID, and 
 *     return false.
 *
 *   If not matched, delete the Item ID serialization
 *   cache copy, and all Item ID - disambig cite
 *   copies, and proceed as if not found.
 *
 * If not found, open an entry mapping this Item ID to
 * its serialization, and another mapping this disambig 
 * to its serialization under the Item ID, and return false.
 *
 *  cache = {
 *   <itemID>: {
 *     serialization: "{title:\"I Am a Fiction\" ... etc etc}",
 *     disambig_form: {
 *       <disambig_serialization>: "(Smith 2000)"
 *     }
 *  }
 */

// Interesting idea, but this won't work. Adding a cite with
// an author having the same family name and a different
// given name should change a cite; the disambig parameters
// serialized here don't account for that possibility,
// causing many tests to fail.

// Pulled from api_cite.js

	//var cacheCopy = this.registry.getCache(Item, disambig);
	//if (cacheCopy) {
	//	return cacheCopy;
	//}

	//this.registry.setCache(Item, disambig, ret);

// Pulled from load.js

//	VARIABLES: {
//		"author":"creator",
//		"collection-editor":"creator",
//		composer:"creator",
//		"container-author":"creator",
//		editor:"creator",
//		"editorial-director":"creator",
//		interviewer:"creator",
//		"original-author":"creator",
//		recipient:"creator",
//		translator:"creator",
//		accessed:"date",
//		container:"date",
//		"event-date":"date",
//		issued:"date",
//		"original-date":"date",
//		submitted:"date",
//		multi:"multi",		
//		"abstract":"string",
//		annote:"string",
//		archive:"string",
//		archive_location:"string",
//		"archive-place":"string",
//		authority:"string",
//		"call-number":"string",
//		"chapter-number":"string",
//		"citation-label":"string",
//		"collection-number":"string",
//		"collection-title":"string",
//		"container-title":"string",
//		DOI:"string",
//		edition:"string",
//		"event-place":"string",
//		event:"string",
//		genre:"string",
//		ISBN:"string",
//		issue:"string",
//		jurisdiction:"string",
//		medium:"string",
//		note:"string",
//		number:"string",
//		"number-of-pages":"string",
//		"number-of-volumes":"string",
//		"original-publisher":"string",
//		"original-publisher":"string",
//		"original-publisher-place":"string",
//		"original-title":"string",
//		page:"string",
//		"page-first":"string",
//		publisher:"string",
//		"publisher-place":"string",
//		section:"string",
//		status:"string",
//		title:"string",
//		URL:"string",
//		version:"string",
//		volume:"string",
//		language:"string",
//		journalAbbreviation:"string",
//		shortTitle:"string",
//		type:"string"
//	},

// if (!CSL.Registry.prototype.getCache) {
//	load("./src/cache.js");
// }


CSL.Registry.prototype.getCache = function (Item, disambig) {
	var item_serialization = this._serializeItem(Item);
	this._disambig_serialization = this._serializeDisambig(disambig);
	//print("disambig: "+this._disambig_serialization);
	if (this.cache[Item.id] && this.cache[Item.id].serialization === item_serialization) {
		//print("Cache return");
		return this.cache[Item.id].disambig_form[this._disambig_serialization];
	} else {
		//print("Cache init");
		this.cache[Item.id] = {}
		this.cache[Item.id].serialization = item_serialization;
		this.cache[Item.id].disambig_form = {};
	}
};

/*
 *
 */
CSL.Registry.prototype.setCache = function (Item, disambig, str) {
	var serialization = this.cache[Item.id].serialization;
	if (!this.cache[Item.id].disambig_form[this._disambig_serialization]) {
		this.cache[Item.id].disambig_form[this._disambig_serialization] = str;
	}
	this._disambig_serialization = false;
};

CSL.Registry.prototype._serializeItem = function (Item) {
	var strings = [];
	var creators = [];
	var dates = [];
	for (var key in Item) {
		switch (CSL.VARIABLES[key]) {
		case "string":
			strings.push(key);
			break;
		case "creator":
			creators.push(key);
			break;
		case "date":
			dates.push(key);
			break;
		}
	}
	strings.sort();
	creators.sort();
	dates.sort();
	var lst = [];

	// Serialize string and number variables
	for (var i = 0, ilen = strings.length; i < ilen; i += 1) {
		lst.push(strings[i]+":\""+Item[strings[i]]+"\"");
	}

	// Serialize creator variables (remember multi!)
	var creator_vars = [];
	if (creators.length) {
		for (var i = 0, ilen = creators.length; i < ilen; i += 1) {
			var creators_lst = [];
			for (var j = 0, jlen = Item[creators[i]].length; j < jlen; j += 1) {
				var namepart_lst = [];
				this._pushNames(Item[creators[i]][j], namepart_lst);
				if (Item[creators[i]][j].multi) {
					var multi_lst = [];
					for (var key in Item[creators[i]][j].multi._key) {
						multi_lst.push(key);
					}
					multi_lst.sort();
					multi_lnames = [];
					for (var k = 0, klen = multi_lst.length; k < klen; k += 1) {
						var multi_nparts = [];
						var name = Item[creators[i]][j].multi._key[multi_lst[k]];
						this._pushNames(name, multi_nparts);
						multi_lnames.push(multi_lst[k]+":[{" + multi_nparts.join("},{") + "}]");
					}
					namepart_lst.push("multi:{" + multi_lnames.join(",") + "}");
				}
				creators_lst.push(namepart_lst.join(","));
			}
			creator_vars.push(creators[i]+":[{" + creators_lst.join("},{") + "}]");
		}
		lst.push(creator_vars.join(","));
	}

	// Serialize date variables
	if (dates.length) {
		for (var i = 0, ilen = dates.length; i < ilen; i += 1) {
			var date_lst = [];;
			for (var key in Item[dates[i]]) {
				// Needs to be fixed: the nested lists in date-parts need
				// to be unrolled properly.
				switch (key) {
				case "raw":
					date_lst.push("raw:\"" + Item[dates[i]].raw + "\"");
					break;
				case "date-parts":
					date_lst.push("\"date-parts\":[" + Item[dates[i]]["date-parts"] + "]");
					break;
				case "circa":
					date_lst.push("circa:\"" + Item[dates[i]].circa + "\"")
					break;
				}
			}
			lst.push(date_lst.join(","));
		}
	}

	// Serialize multi string variables
	if (Item.multi && Item.multi._keys) {
		var multifields = [];
		var multi_flst = [];
		for (var ikey in Item.multi._keys) {
			multi_flst.push(ikey);
		}
		multi_flst.sort();
		for (var i = 0, ilen = multi_flst.length; i < ilen; i += 1) {
			var multilangs = [];
			multi_llst = [];
			for (var jkey in Item.multi._keys[multi_flst[i]]) {
				multi_llst.push(jkey);
			}
			multi_llst.sort();
			for (var j = 0, jlen = multi_llst.length; j < jlen; j += 1) {
				multilangs.push(multi_llst[j] + ":\"" + Item.multi._keys[multi_flst[i]][multi_llst[j]] + "\"");
			}
			multifields.push(multi_flst[i]+":{" + multilangs.join("},{") + "}");
		}
		lst.push("multi:{" + multifields.join("},{") + "}");
	}

	str = "{" + lst.join(",") + "}";
	//print(str);
	return str;
};


CSL.Registry.prototype._pushNames = function (name, lst) {
	for (var k = 0, klen = CSL.NAME_PARTS.length; k < klen; k += 1) {
		if (name[CSL.NAME_PARTS[k]]) {
			lst.push(CSL.NAME_PARTS[k] + ":\"" +name[CSL.NAME_PARTS[k]]+"\"");
		}
	}
};


CSL.Registry.prototype._serializeDisambig = function (disambig) {
	var str = "{";
	if (disambig) {
		str += "names:"+disambig.names+",";
		// Needs to be fixed: the nested number lists inside givens
		// need to be unrolled properly.
		str += "givens:"+disambig.givens+",";
		str += "year_suffix:\""+disambig.year_suffix+"\",";
		str += "disambiguate:\""+disambig.disambiguate+"\",";
	}
	str += "}"
	//print(str);
	return str;
};
