<?php

/**
 * Ported from citeproc.js by Adam Vessey
 * FIXME:  Needs testing!
 */

class CSL_DateParser {
  static public function getInstance($csl_date_parts_all = NULL, $clean = TRUE) {
    static $instance = NULL;
    if (is_null($instance)) {
      $instance = new CSL_DateParser();
    }
    if ($clean) {
      $instance->setOrderMonthDay();
      $instance->resetMonths();
    }
    if (isset($csl_date_parts_all)) {
      $instance->set_parts($csl_date_parts_all);
    }
    return $instance;
  }
  
  protected $jiy, $jiysplitter, $jiymatcher, $jmd, $jy, $jr, $rexdash,
    $rexdashslash, $rexslashdash, $seasonrexes, $mstrings, $use_array = TRUE,
    $monthguess, $dayguess, $msets, $mabbrevs, $mrexes, $csl_date_parts_all;
  
  private function set_parts($date_parts) {
    $this->csl_date_parts_all = $date_parts;
  }
  
  private function __construct() {
    $this->jiy = array(
      "\x{660E}\x{6CBB}" => 1867,
      "\x{5927}\x{6B63}" => 1911,
      "\x{662D}\x{548C}" => 1925,
      "\x{5E73}\x{6210}" => 1988
    );
    
    $jiymatchstring = array_keys($this->jiy);
    $jiymatchstring = implode('|', $jiymatchstring);
    
    $this->jiysplitter = "/(?:". $jiymatchstring .")(?:[0-9]+)/u";
    $this->jiymatcher = "/(?:". $jiymatchstring .")(?:[0-9]+)/u"; //Global!

    $this->jmd = "/(\x{6708}|\x{5E74})/u"; //Global!
    $this->jy = "/\x{65E5}/u";
    $this->jr = "/\x{301c}/u"; //Global!
    
    //%%NUMD%% and %%DATED%% just seem to be templates
    $yearlast = "(?:[?0-9]{1,2}%%NUMD%%){0,2}[?0-9]{4}(?![0-9])";
    $yearfirst = "[?0-9]{4}(?:%%NUMD%%[?0-9]{1,2}){0,2}(?![0-9])";
    $number = "[?0-9]{1,3}";
    $rangesep = "[%%DATED%%]";
    $fuzzychar = "[?~]";
    $chars = "[a-zA-Z]+";
    $rex = "($yearfirst|$yearlast|$number|$rangesep|$fuzzychar|$chars)";
    $this->rexdash = preg_replace(array('/%%NUMD%%/', '/%%DATED%%/'), array('-', '-'), $rex);
    $this->rexdashslash = preg_replace(array('/%%NUMD%%/', '/%%DATED%%/'), array('-', '\/'), $rex);
    $this->rexslashdash = preg_replace(array('/%%NUMD%%/', '/%%DATED%%/'), array('\/', '-'), $rex);

    $seasonstrs = array();
    $this->seasonrexes = array();

    foreach($seasonstrs as $season) {
      //FIXME:  Might have to push, instead of 'enqueueing'
      $this->seasonrexes[] = $season .'.*';
    }
    
    $this->mstrings = 'january february march april may june july august september october november december spring summer fall winter spring summer';
    $this->mstrings = explode(' ', $this->mstrings);
    
    $this->setOrderMonthDay();
  }
  
	
	private function setOrderDayMonth() {
		$this->monthguess = 1;
		$this->dayguess = 0;
	}
	private function setOrderMonthDay() {
		$this->monthguess = 0;
		$this->dayguess = 1;
	}

  private function resetMonths() {
		$this->msets = array();
    foreach ($this->mstrings as $mstring) {
      $this->msets[] = array($mstring);
    }

		$this->mabbrevs = array();
    foreach ($this->msets as $set) {
      $temp = array();
      foreach ($set as $j) {
        $temp[] = substr($j, 0, 3);
      }
      $this->mabbrevs[] = $temp;
    }
    
		$this->mrexes = array();
    foreach ($this->mabbrevs as $i) {
      $this->mrexes[] = "(?:". implode('|', $i) .')';
    }
	}
  
  private function addMonths($lst) {
    if (is_string($lst)) {
      $lst = preg_split('/\s+/');
    }
    if (count($lst) !== 12 && count($lst) !== 16) {
      //TODO:  Throw some kinda error message...
      return;
    }
    
    //$othermatch = array(); //Not used?
    //$thismatch = array(); //Not used?
    $mab =& $this->mabbrevs;
    
    foreach ($lst as $key_i => $val_i) {
      $abbrevlen = FALSE;
      $skip = FALSE;
      $insert = 3;
      $extend = array();
      foreach ($mab as $key_j => $val_j) {
        if ($key_j === $key_i) {
          foreach ($mab[$key_i] as $key_k => $val_k) {
            if ($val_k === substr($val_i, 0, strlen($val_k))) {
              $skip = TRUE;
              break;
            }
          }
        }
        else {
          foreach ($mab[$key_j] as $key_k => $val_k) {
            $abbrevlen = strlen($val_k);
            if ($val_k === substr($val_i, 0, $abbrevlen)) {
              $mset_jk =& $this->msets[$key_j][$key_k];
              while (substr($mset_jk, 0, $abbrevlen) === substr($val_i, 0, $abbrevlen)) {
                if ($abbrevlen > strlen($val_i) || $abbrevlen > strlen($mset_jk)) {
                  //TODO:  Throw some kinda error about month parsing...
                  break;
                }
                else {
                  $abbrevlen += 1;
                }
              }
              $insert = $abbrevlen;
              $extend[$key_j][$key_k] = $abbrevlen;
            }
          }
        }
        foreach ($extend as $key_1 => $val_1) {
          //Note: Skipped some int parsing here, shouldn't affect anything?
          foreach ($val_1 as $key_2 => $val_2) {
            $mab[$key_1][$key_2] = substr($this->msets[$key_1][$key_2], 0, $val_2);
          }
        }
      }
      if (!$skip) {
        $this->msets[$key_i][] = $val_i;
        $mab[$key_i][] = substr($val_i, 0, $insert);
      }
    }
    
    $this->mrexes = array();
    foreach ($mab as $val) {
      $this->mrexes[] = '(?:'. implode('|', $val) .')';
    }
  }

  public function parse($txt) {
    $slash = $dash = FALSE;
    $range_delim = $date_delim = NULL;
    
    if (preg_match($this->jmd, $txt) > 0) {
      $txt = preg_replace($this->jy, '', $txt);
      $txt = preg_replace($this->jmd, '-', $txt);
      $txt = preg_replace($this->jr, '/', $txt);
      $txt = preg_replace('/-\//', '/', $txt);
      $txt = preg_replace('/-$/', '', $txt); //Might have to verify this?
      $slst = preg_split($this->jiysplitter, $txt);
      $lst = array();
      $mm = array();
      preg_match_all($this->jiymatcher, $txt, $mm);
      $mmx = array();
      foreach ($mm as $match) {
        $matches = array();
        preg_match_all('/([^0-9]+)([0-9]+)/', $match, $matches);
        $mmx = array_merge($mmx, array_slice($matches, 1));
      }
      foreach ($slst as $key => $val) {
        $lst[] = $val;
        if ($key !== count($slst) - 1) {
          $mmpos = $key * 2;
          $lst[] = $mmx[$mmpos];
          $lst[] = $mmx[$mmpos + 1];
        }
      }
      foreach ($lst as $key => $item) {
        $lst[$key + 1] = $this->jiy[$lst[$key]] + intval($lst[$key + 1], 10);
        $lst[$key] = '';
      }
      
      $txt = implode('', $lst);
      $txt = preg_replace('/\s*-\s*$/', '', $txt);
      $txt = preg_replace('/\s*-\s*\//', '/', $txt);
      $txt = preg_replace('/\.\s*$/', '', $txt);
      $txt = preg_replace('/\.(?! )/', '', $txt);
      
      $slash = strpos($txt, '/');
      $dash = strpos($txt, '-');
    }
    $txt = preg_replace('/([A-Za-z])\./', '$1', $txt);
    $number = $note = NULL;
    $thedate = array();
    $suff = '';
    
    if (substr($txt, 0, 1) === '"' && substr($txt, -1) === '"') {
      $thedate['literal'] = substr($txt, 1, -1);
      return $thedate;
    }
    elseif ($slash !== FALSE && $dash !== FALSE) {
      $slashes = explode('/', $txt);
      if (count($slashes) > 3) {
        $range_delim = '-';
        $date_delim = '/';
        $lst = preg_split($this->rexslashdash, $txt);
      }
      else {
        $range_delimg = '/';
        $date_delim = '-';
        $lst = preg_split($this->rexdashslash, $txt);
      }
    }
    else {
      $txt = preg_replace('/\//', '-', $txt);
      $range_delim = '-';
      $date_delim = '-';
      $lst = preg_split($this->rexdash, $txt);
    }
    
    $ret = array();
    foreach ($lst as $val) {
      $match = NULL;
      if (preg_match('/^\s*([\-\/]|[a-zA-Z]+|[\-~?0-9]+)\s*$/', $val, $match) > 0) {
        $ret[] = $match[1]; 
      }
    }
    
    $delim_pos = array_search($range_delim, $ret);
    $delims = array();
    $is_range = FALSE;
    if ($delim_pos !== FALSE) {
      $delims[] = array(0, $delim_pos);
      $delims[] = array($delim_pos + 1, count($ret));
      $is_range = TRUE;
    }
    else {
      $delims[] = array(0, count($ret));
    }
   
    foreach ($delims as $delim) {
      $date = array_slice($ret, $delim[0], $delim[1]);
      foreach ($date as $element) {
        $lc = strtolower($element); //element.toLocaleLowerCase...
        
        if (strpos($element, $date_delim) !== FALSE) {
          //TODO
          $this->parseNumericDate($thedate, $date_delim, $suff, $element);
          continue;
        }
        elseif (preg_match('/[0-9]{4}/', $element) > 0) {
          $thedate['year'. $suff] = preg_replace('/^0*/', '', $element);
        }
        
        $breakme = FALSE;
        
        foreach ($this->mrexes as $key => $mrex) {
          //element.toLocaleLowerCase.match($mrex)
          if (preg_match($mrex, $lc) > 0) {
            $thedate['month'. $suff] = '' + ($key + 1);
            $breakme = TRUE;
            break;
          }
          elseif ($breakme) {
            continue;
          }
          elseif (preg_match('/^[0-9]+$/', $lc)) {
            $number = intval($element);
          }
          
          if ($number && preg_match('/^bc/', $element) > 0) {
            $thedate['year'. $suff] = ''. ($number * -1);
						$number = '';
						continue;
          }
          elseif ($number && preg_match('/^ad/', $element) > 0) {
            $thedate['year'. $suff] = ''. $number;
						$number = '';
						continue;
          }
        }
        $breakme = FALSE;
        
        foreach ($this->seasonrexes as $key => $srex) {
          if (preg_match($srex, $lc) > 0) {
            $thedate['season'. $suff] = ''. ($key + 1);
            $breakme = TRUE;
            break;
          }
        }
        if ($breakme) {
          continue;
        }
        if ($element === '~' || $element === '?' || $element === 'c' || preg_match('/^cir/', $element) > 0) {
          $thedate['circa'] = 1;
          continue;
        }
        if(preg_match('/(?:mic|tri|hil|eas)/', $lc) > 0 && !in_array('season'. $suff, $thedate)) {
          $note = $element;
          continue;
        }
      }
      if ($number) {
        $thedate['day'. $suff] = $number;
        $number = NULL;
      }
      if (isset($note) && !in_array('season'. $suff, $thedate)) {
        $thedate['season'. $suff] = $note;
        $note = NULL;
      }
      $suff = '_end';
    }
    if ($is_range) {
      //TODO:  Get CSL date part stuff...  Might have to iterate over this differently?
      foreach ($this->csl_date_parts_all as $item) {
        if (in_array($item, $thedate) && !in_array($item .'_end', $thedate)) {
          $thedate[$item .'_end'] = $thedate[$item];
        }
        elseif (!in_array($item, $thedate) && in_array($item .'_end', $thedate)) {
          $thedate[$item] = $thedate[$item .'_end'];
        }
      }
    }
    if (!in_array('year', $thedate)) {
      $thedate = array('literal' => $txt);
    }
    
    if ($this->use_array) {
      return $this->toArray($thedate);
    }
    else {
      return $thedate;
    }
  }
  
  public function returnAsArray() {
    $this->use_array = TRUE;
  }
  
  public function returnAsKeys() {
    $this->use_array = FALSE;
  }
  
  private function toArray($thedate) {
    //dd($thedate, 'toArray');
    $toReturn = array('date-parts' => array());
    
    if (array_key_exists('literal', $thedate)) {
      $toReturn = array('literal' => $thedate['literal']);
    }
    else {
      $start_parts = array('year', 'month', 'day');
      $end_parts = array('year_end', 'month_end', 'day_end');
      $start = array();
      $end = array();
      foreach ($start_parts as $part) {
        if (!in_array($part, $thedate)) {
          break;
        }
        else {
          $start[] = $thedate[$part];
        }
      }
      foreach ($end_parts as $part) {
        if (!in_array($part, $thedate)) {
          break;
        }
        else {
          $end[] = $thedate[$part];
        }
      }
      $toReturn['date-parts'][] = $start;
      $toReturn['date-parts'][] = $end;
    }
    
    //dd($toReturn, 'Fully parsed stuff...');
    //dd($this, 'Parser object');
    return $toReturn;
  }
	
  private function parseNumericDate(&$ret, $delim, $suff, $txt) {
    $lst = preg_split($delim, $txt);
    foreach ($lst as $key => $val) {
      if (strlen($val, 4)) {
        $ret['year'. $suff] = preg_replace('/^0*/', '', $txt);
        if ($key === 0) {
          $lst = array_slice($lst, 1);
        }
        else {
          $lst = array_slice($lst, 0, $key);
        }
        break;
      }
    }
    foreach ($lst as &$val) {
      $val = intval($val);
    }
    
    if (count($lst) === 1) {
      $ret['month'. $suff] = ''. $lst[0];
    }
    elseif (count($lst) === 2) {
      if ($lst[$this->monthguess] > 12) { //FIXME:  This doesn't really work...
				$ret['month'. $suff] = ''. $lst[$this->dayguess];
				$ret['day'. $suff] = ''. $lst[$this->monthguess];
			} 
      else {
				$ret['month'. $suff] = ''. $lst[$this->monthguess];
				$ret['day'. $suff] = ''. $lst[$this->dayguess];
			}
    }
  }
}
?>
