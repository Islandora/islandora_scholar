/*
 * Copyright (c) 2009 and 2010 Frank G. Bennett, Jr. All Rights Reserved.
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
 * Copyright (c) 2009 and 2010 Frank G. Bennett, Jr. All Rights Reserved.
 */

//load("./dojo/dojo/dojo.js");
//dojo.registerModulePath("dojo","./dojo/dojo");
//dojo.registerModulePath("dojox","./dojo/dojox");
//dojo.registerModulePath("csl","./src");
//dojo.registerModulePath("doh","./dojo/util/doh");

//dojo.require("doh.runner");

dojo.provide("citeproc_js.speed");

//dojo.require("csl.csl");

doh.registerGroup("citeproc_js.speed",
	[
		function testSpeed(){
			CSL.debug("iterate over sorted list and print");
			citeproc_js.speed.registry.iterate();
		}

	],
	function(){ //setup

		CSL.debug("loading functions");

		var Registry = function(){
			this.registry = new Object();
			this.start = false;
			this.end = false;
			this.initialized = false;
			this.skip = false;
			this.maxlength = 0;
			this.insert = function(newitem){
				if (newitem.phoneytitle.length > this.maxlength){
					this.maxlength = newitem.phoneytitle.length;
				}
				// if the registry is empty, initialize it with
				// this object.
				if (!this.initialized){
					this.registry[newitem.id] = newitem;
					this.start = newitem.id;
					this.end = newitem.id;
					this.initialized = true;
					return;
				}
				// if this object is less than the first one,
				// insert it as the first.
				if (-1 == this.compareStrings(newitem.phoneytitle,this.registry[this.start].phoneytitle)){
					//CSL.debug("Sequence at the beginning: "+newitem.phoneytitle+", "+this.registry[this.start].phoneytitle);
					newitem.next = this.registry[this.start].id;
					this.registry[this.start].prev = newitem.id;
					newitem.prev = false;
					this.start = newitem.id;
					this.registry[newitem.id] = newitem;
					return;
				}
				// if this object is greater than the
				// last one, insert it as the last.
				if (-1 == this.compareStrings(this.registry[this.end].phoneytitle,newitem.phoneytitle)){
					//CSL.debug("Sequence at the end: "+this.registry[this.end].phoneytitle+", "+newitem.phoneytitle);
					newitem.prev = this.registry[this.end].id;
					this.registry[this.end].next = newitem.id;
					newitem.next = false;
					this.end = newitem.id;
					this.registry[newitem.id] = newitem;
					return;
				}
				//
				// if we reach this, it's safe to iterate
				var curr = this.registry[this.start];
				while (true){
					// compare the new token to be added with
					// the one we're thinking about placing it before.
					var cmp = this.compareStrings(curr.phoneytitle,newitem.phoneytitle);
					if (cmp > 0){
						// insert mid-list, before the tested item
						//CSL.debug("Inserting mid-list: "+newitem.phoneytitle+", next is "+curr.phoneytitle);
						this.registry[curr.prev].next = newitem.id;
						newitem.prev = curr.prev;
						newitem.next = curr.id;
						curr.prev = newitem.id;
						this.registry[newitem.id] = newitem;
						return;
					} else if (cmp == 0) {
						// also insert before, but this is a duplicate,
						// so we need to provide for cases where the
						// inserted object ends up at the beginning of
						// the virtual list.
						//
						//CSL.debug("I'm a dupe: "+curr.phoneytitle);
						// (disambiguation handling would slot in here)
						//
						if (false == curr.prev){
							newitem.prev = false;
							newitem.next = curr.id;
							curr.prev = newitem.id;
							this.registry[newitem.id] = newitem;
							return;
						} else {
							this.registry[curr.prev].next = newitem.id;
							newitem.prev = curr.prev;
							newitem.next = curr.id;
							curr.prev = newitem.id;
							this.registry[newitem.id] = newitem;
							return;
						}
					}
					curr = this.registry[curr.next];
				};
			};

			this.compareStrings = function(a,b){
				return a.toLocaleLowerCase().localeCompare(b.toLocaleLowerCase());
			};

			this.iterate = function(){
				var curr = this.registry[this.start];
				//CSL.debug("starting iterate at: "+printme);
				while(curr.next != false){
					var printme = curr.phoneytitle;
					//while (printme.length < this.maxlength){
					//	printme = " "+printme;
					//}
					CSL.debug(printme);
					curr = this.registry[curr.next];
				}
			};
		};

		var makeThing = function(key,phoneytitle,transfat){
			this.id = key;
			this.phoneytitle = phoneytitle;
			this.transfat = transfat;
			this.next = false;
			this.prev = false;
		};

		function makePhoneyTitle(){
			var ret = "";
			var mylen = (Math.floor(Math.random()*25)+1);
			for (var i=0; i<mylen; i++){
				var unival = (Math.floor(Math.random()*126)+257);
				unival = unival.toString(16);
				while (unival.length < 4){
					unival = "0"+unival;
				}
				eval("unival = \"\\u"+unival+"\"");
				ret += unival;
			}
			return ret;

		}

		CSL.debug("instantiating registry object");
		citeproc_js.speed.registry = new Registry();

		CSL.debug("loading items to registry");
		for (var i=0; i<1000; i++){
			var key = "item"+i;
			var phoneytitle = makePhoneyTitle();
			var transfat = "All work and no play makes Jack a dull boy."
					  + "All work and no play makes Jack a dull boy."
					  + "All work and no play makes Jack a dull boy."
					  + "All work and no play makes Jack a dull boy."
					  + "All work and no play makes Jack a dull boy."
					  + "All work and no play makes Jack a dull boy."
				+ "All work and no play makes Jack a dull boy.";
			var thing = new makeThing(key,phoneytitle,transfat);
			citeproc_js.speed.registry.insert(thing);
		}
	},
	function(){ //teardown

	}
);

//citeproc_js.run();
