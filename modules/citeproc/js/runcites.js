// Renders each citation.
Drupal.behaviors.citeproc = function (context){
  if(Drupal.settings.citeproc !== undefined && Drupal.settings.citeproc.refresh) {
    Drupal.settings.citeproc.refresh = false;
    var appendCitations = function (citeproc, items) {
      for(id in items) {
        var citation = items[id];
        var output = citeproc.appendCitationCluster(citation, true);
        if (output[0] && output[0].length && output[0][1].length){
          jQuery('#' + id + '.citeproc-in-text').html(output[0][1]);
        }
      }
    }
    var sys = new citeproc_sys();
    var bibliographies = sys.retrieveBibliographies();
    for(id in bibliographies) {
      var bibliography = bibliographies[id];  
      var style = sys.retrieveStyle(bibliography.style);
      var citeproc = new CSL.Engine(sys, style);
      var abbreviations = bibliography.abbreviation;
      if(abbreviations !== null) {
        citeproc.setAbbreviations(abbreviations);
      }
      appendCitations(citeproc, bibliography.items);
      var output = citeproc.makeBibliography();
      if (output && output.length && output[1].length){
        output = output[0].bibstart + output[1].join("") + output[0].bibend;
        jQuery('#' + bibliography.id + '.citeproc-bibliography').html(output); 
      }
    }
  }
}
	