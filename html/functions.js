// data
strShowEditBefore = '<img src="right.png" alt="right"/>Edit';
strShowEditAfter = '<img src="downleft.png" alt="down & left"/>Edit';
strShowNoteBefore = '<img src="right.png" alt="right"/>Note';
strShowNoteAfter = '<img src="downleft.png" alt="down & left"/>Note';
strQuickCognateSet = 'Q.C.S.';

// cognate sets
strShowCorrBefore = '<img src="right.png" alt="right"/>Corr';
strShowCorrAfter = '<img src="downleft.png" alt="down & left"/>Corr';
strHighlightData = '<img src="up.png" alt="up"/>Data';
strChangeCognateSet = 'alter';
strHighlightCorrespondencesFromCognates = '<img src="down.png" alt="down"/>Corr';
strHighlightCorrespondencesReconstructionsFromCognates = '<img src="doubledown.png" alt="doubledown"/>Recon';
strShowReconBefore = '<img src="right.png" alt="right"/>Proto';
strShowReconAfter = '<img src="downleft.png" alt="down & left"/>Proto';

// correspondences
strHighlightCognates = '<img src="up.png" alt="up"/>Cog';
strRemoveSelected = 'Rm Sel Cog';
strEditBefore = '<img src="right.png" alt="right"/>Edit';
strEditAfter = '<img src="downleft.png" alt="down & left"/>Edit';
strReconBefore = '<img src="right.png" alt="right"/>Recon';
strReconAfter = '<img src="downleft.png" alt="down & left"/>Recon';
strHighlightReconstructions = '<img src="down.png" alt="down"/>Recon';
strChangeBefore = '<img src="right.png" alt="right"/>Change';
strChangeAfter = '<img src="downleft.png" alt="down & left"/>Change';

// reconstructions
strViewCorrespondences = '<img src="up.png" alt="up"/>Corr';
strPlusCognates = '<img src="doubleup.png" alt="doubleup"/>Cog';
strReconRemoveSelected = 'Rm Sel Corr';
strViewProtoform = '<img src="up.png" alt="up"/>Protoforms';
strSCAssoc = 'Assoc with protoform';
strSCDisassoc = 'Disassoc with protoform';
strSCAssoc = 'Associate';
strSCDisassoc = 'Disassociate	';
strReconRemoveSelected = 'Rm Sel Corr';

function isArray(testObject) {; 
  return testObject && !(testObject.propertyIsEnumerable('length')) && typeof testObject === 'object' && typeof testObject.length === 'number';
}

drawDataTable = function drawDataTable()
{
	if(top.dataFrame==null || top.dataFrame.contentDocument==null || top.dataFrame.contentDocument.getElementById("dataTable")==null) { return; }
	
	htmlstr="";

	if(top.dataFrameHighlightedSource != null)
	{
		for(var i in top.languages)
		{
			top.selections[top.languages[i].id] = Array();

			if(top.item_cognate_dep[top.dataFrameHighlightedSource] != null && top.item_cognate_dep[top.dataFrameHighlightedSource][top.languages[i].id] != null)
			{
				for(var j in top.item_cognate_dep[top.dataFrameHighlightedSource][top.languages[i].id])
				{
					top.selections[top.languages[i].id][j] = 1*top.item_cognate_dep[top.dataFrameHighlightedSource][top.languages[i].id][j];
				}
			}
		}
	}
	
	if(top.col_order.length!=top.languages.length || top.col_visible.length!=top.languages.length)
	{
		top.initializeOrderVisibility();
	}


	if(top.dataEditOpen.length!=top.items.length)
	{
		top.initializeDataHiddenRows();
	}

	var nr = top.items.length;
	var nc = top.languages.length;

	if(nr == 0)
	{
		top.dataFrame.contentDocument.getElementById("dataTable").innerHTML = "<p><em>No data has been entered yet.<\/em><\/p>";
		return;
	}
	
	if(top.invalidateDataSort)
	{
		sortData(top.dataSort);
		top.invalidateDataSort=0;
	}

	htmlstr += '<table class="data_table">';
	if(!top.useAbbreviations)
	{
		htmlstr += '<tr id="fullname"><th><\/th>';
		for(j=0;j<nc;j++)
		{
			if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
			{
				htmlstr += '<th>'+top.languages[top.col_order[j]].fullname+'<\/th>';
			}
		}
		htmlstr += '<\/tr>';
	}
	else
	{
		htmlstr += '<tr id="abbr"><th><\/th>';
		for(j=0;j<nc;j++)
		{
			if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
			{
				htmlstr += '<th>'+top.languages[top.col_order[j]].abbr+'<\/th>';
			}
		}
		htmlstr += '<\/tr>';
	}

	htmlstr += '<tr><th><\/th>';
	for(j=0;j<nc;j++)
	{
		if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
		{
			htmlstr += '<td class="colutility"><a href="javascript:" title="Clear the selection from this column" onclick="deselect('+top.languages[top.col_order[j]].id+');">clear<\/a><\/td>';
		}
	}
	htmlstr += '<\/tr>';

	htmlstr += '<tr><td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortData(null); top.drawDataTable();">sort<\/a><\/td>';
	for(j=0;j<nc;j++)
	{
		if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
		{
			htmlstr += '<td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortData('+(languages[top.col_order[j]].id)+'); top.drawDataTable();">sort<\/a><\/td>';
		}
	}
	htmlstr += '<\/tr>';

	for (var i in top.dataRowOrder)
	{
		row = top.dataRowOrder[i];
		htmlstr += '<tr>';
		htmlstr += '<td class="gloss plaincursor"><div id="scroll'+row+'"><\/div>'+top.lemmas[row]+'<\/td>';

		for(j=0;j<nc;j++)
		{
			if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
			{
				if(top.languages[top.col_order[j]].color.length > 0)
				{
					color = top.languages[top.col_order[j]].color;
				}
				else
				{
					color = "default";
				}

				if(top.selections[top.languages[top.col_order[j]].id].indexOf(row)!=-1) { stylestring = ' class="plaincursor selected"'; } else { stylestring=' class="plaincursor"'; }
				if(top.items[row][top.languages[top.col_order[j]].id] != null)
				{
					str = top.items[row][top.languages[top.col_order[j]].id];
				}
				else { str = ''; }
				htmlstr += '<td id="'+row+','+languages[top.col_order[j]].id+'" style="background-color: '+color+';" onclick="cellSelect(this.id);" '+stylestring+'>'+str+'<\/td>';
			}
		}

		if(dataEditOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit the data" onclick="toggleDataEdit('+row+')" id="elink'+row+'">'+strShowEditAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit the data" onclick="toggleDataEdit('+row+')" id="elink'+row+'">'+strShowEditBefore+'<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Create a quick cognate set, using all of the forms associated with this lemma" onclick="quickCognateSet('+row+')">'+strQuickCognateSet+'<\/a><\/td>';
		htmlstr += '<\/tr>';

		// edit
		if(dataEditOpen[row])
			htmlstr += '<tr id="edit'+row+'" class="utility"><th><\/th>';
		else
			htmlstr += '<tr id="edit'+row+'" class="utility" style="display: none"><th><\/th>';
		for(j=0;j<nc;j++)
		{
			if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
			{
				if(top.selections[top.languages[top.col_order[j]].id].indexOf(row)!=-1) { stylestring = ' class="plaincursor selected"'; } else { stylestring="plaincursor"; }
				if(top.items[row][top.languages[top.col_order[j]].id] != null)
				{
					str = top.items[row][top.languages[top.col_order[j]].id];
				}
				else { str = ''; }
				htmlstr += '<td id="'+i+','+(top.col_order[j]+1)+'" '+stylestring+'><input type="text" id="editinput'+row+','+top.languages[top.col_order[j]].id+'" value="'+str+'" class="singlesegmentinput"/><\/td>';
			}
		}
		htmlstr += '<td colspan="2" class="button"><a href="javascript:" title="Save the values that you have entered" onclick="saveEdit('+row+')">save<\/a><\/td>';
		htmlstr += '<\/tr>';

	}

	htmlstr += '<\/table>';
	top.dataFrame.contentDocument.getElementById("dataTable").innerHTML = htmlstr;

//	alert(top.dataRowOrder);
//	alert(top.selections[top.languages[0].id]);
}


drawCognateSetTable = function drawCognateSetTable()
{
	if(top.cognateFrame==null || top.cognateFrame.contentDocument==null || top.cognateFrame.contentDocument.getElementById("dataTable")==null) { return; }

	htmlstr="";
	
	if(top.cognate_labels.length == 0)
	{
		top.cognateFrame.contentDocument.getElementById("dataTable").innerHTML = "<p><em>No cognate sets defined so far.<\/em><\/p>";
		return;
	}

	if(top.col_order.length!=top.languages.length || top.col_visible.length!=top.languages.length)
	{
		top.initializeOrderVisibility();
	}
	
	nvislanguages=0;
	htmlstr += '<table class="data_table">';
	if(!top.useAbbreviations)
	{
		htmlstr += '<tr><th><\/th><th><\/th>';
		for(j=0;j<top.languages.length;j++)
		{
			if(top.col_visible[top.col_order[j]])
			{
				nvislanguages++;
				htmlstr += '<th>'+top.languages[top.col_order[j]].fullname+'<\/th>';
			}
		}
		htmlstr += '<\/tr>';
	}
	else
	{
		htmlstr += '<tr id="abbr"><th><\/th><th><\/th>';
		for(j=0;j<top.languages.length;j++)
		{
			if(top.col_visible[top.col_order[j]])
			{
				nvislanguages++;
				htmlstr += '<th>'+top.languages[top.col_order[j]].abbr+'<\/th>';
			}
		}
		htmlstr += '<\/tr>';
	}

	if(top.invalidateCognateSort)
	{
		sortCognateSets(top.cognateSort);
		top.initializeCognateHiddenRows(); // test
		top.invalidateCognateSort=0;
	}

	htmlstr += '<tr><th><\/th><td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortCognateSets(null); top.drawCognateSetTable();">sort<\/a><\/td>';
	for(j=0;j<top.languages.length;j++)
	{
		if(top.col_visible[top.col_order[j]])
		{
			htmlstr += '<td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortCognateSets('+(languages[top.col_order[j]].id)+'); top.drawCognateSetTable();">sort<\/a><\/td>';
		}
	}
	htmlstr += '<\/tr>';
	
	for(var i in top.cognateSetRowOrder)
	{
		row = top.cognateSetRowOrder[i];

		stylestring = "";

		if(top.cognateFrameHighlights.indexOf(row) != -1)
		{
			if(top.cognateFrameSelectedRow == row)
			{
				stylestring = "highlighted_selected";
			}
			else
			{
				stylestring = "highlighted";
			}
		}
		else if(top.cognateFrameSelectedRow == row)
		{
			stylestring = "selected";
		}
		htmlstr += '<tr class="'+stylestring+'">';

		if(top.cognateFrameSelectedRow == row)
		{
			htmlstr += '<th><div id="scroll'+row+'"><\/div><input type="checkbox" checked="checked" onchange="rowselect('+row+')"><\/th>';
		}
		else
		{
			htmlstr += '<th><div id="scroll'+row+'"><\/div><input type="checkbox" onchange="rowselect('+row+')"><\/th>';
		}

		htmlstr += '<td class="gloss">'+top.cognate_labels[row]+'<\/td>';
		for(j=0;j<top.languages.length;j++)
		{
			if(top.col_visible[top.col_order[j]])
			{
				str = '<td><\/td>';

				if(top.languages[top.col_order[j]].color.length > 0)
				{
					color = top.languages[top.col_order[j]].color;
				}
				else
				{
					color = "default";
				}

				if(!top.languages[top.col_order[j]].reconstructed)
				{
					if(top.cognate_sets[row]!=null && top.cognate_sets[row][top.languages[top.col_order[j]].id]!=null)
					{
						str = '<td style="background-color: '+color+';">' + top.cognate_sets[row][top.languages[top.col_order[j]].id] + '<\/td>';
					}
				}
				else
				{
					if(top.protoforms[row]!=null && top.protoforms[row][top.languages[top.col_order[j]].id]!=null)
					{
						str = '<td class="protoform" style="background-color: '+color+';">' + top.protoforms[row][top.languages[top.col_order[j]].id] + '<\/td>';
					}
				}
				htmlstr += str;
			}
		}

		if(top.cognateCorrOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to create a correspondence set" onclick="toggleCorrespondenceForm('+row+')" id="link'+row+'">'+strShowCorrAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to create a correspondence set" onclick="toggleCorrespondenceForm('+row+')" id="link'+row+'">'+strShowCorrBefore+'<\/a><\/td>';		
		if(top.cognateProtoOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to reconstruct a proto-form" onclick="toggleReconstructionForm('+row+')" id="rlink'+row+'">'+strShowReconAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to reconstruct a proto-form" onclick="toggleReconstructionForm('+row+')" id="rlink'+row+'">'+strShowReconBefore+'<\/a><\/td>';
		if(top.cognateNoteOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to enter a note" onclick="toggleNoteForm('+row+')" id="nlink'+row+'">'+strShowNoteAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to enter a note" onclick="toggleNoteForm('+row+')" id="nlink'+row+'">'+strShowNoteBefore+'<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the data associated with this cognate set" onclick="highlightData('+row+')">'+strHighlightData+'<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the correspondence sets associated with this cognate set" onclick="top.highlightCorrespondencesFromCognates('+row+')">'+strHighlightCorrespondencesFromCognates+' ('+top.cognate_correspondence_counts[row]+')<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the reconstructed segments associated with this cognate set" onclick="top.highlightCorrespondencesReconstructionsFromCognates('+row+')">'+strHighlightCorrespondencesReconstructionsFromCognates+' ('+top.cognate_reconstruction_counts[row]+')<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Alter this cognate set to reflect the selections in the data window" onclick="changeCognateSet('+row+')">'+strChangeCognateSet+'<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Remove this cognate set" onclick="removeCognate('+row+');">X<\/a><\/td>';
		htmlstr += '<\/tr>';

		// correspondence
		if(top.cognateCorrOpen[row])
			htmlstr += '<tr id="corr'+row+'" class="utility"><th><\/th><th><\/th>';
		else
			htmlstr += '<tr id="corr'+row+'" class="utility" style="display: none;"><th><\/th><th><\/th>';
		for(j=0;j<top.languages.length;j++)
		{
			if(top.col_visible[top.col_order[j]])
			{
				if(top.cognate_sets[row]!=null && top.cognate_sets[row][top.languages[top.col_order[j]].id]!=null && top.cognate_sets[row][top.languages[top.col_order[j]].id].length>0)
				{
					htmlstr += '<td><input type="text" id="c'+row+','+(top.col_order[j]+1)+'" value="" class="singlesegmentinput"\/><\/td>';
				}
				else
				{
					htmlstr += '<td><\/td>';
				}
			}
		}
		htmlstr += '<td colspan="2"><a href="javascript:" title="Create a new correspondence" onclick="createCorrespondence(\''+row+'\');">create<\/a><\/td>';
		htmlstr += '<td colspan="2"><a href="javascript:" title="Copy the value from the leftmost box to the other boxes" onclick="copyFromFirst(\''+row+'\');">copy from first<\/a><\/td>';
		htmlstr += '<td colspan="2"><a href="javascript:" title="Clear all boxes" onclick="clearCorrespondenceForm(\''+row+'\');">clear<\/a><\/td>';
		htmlstr += '<\/tr>';
		if(top.cognateCorrOpen[row])
			htmlstr += '<tr id="excorr'+row+'" class="utility">';
		else
			htmlstr += '<tr id="excorr'+row+'" class="utility" style="display: none;">';
		htmlstr += '<th><\/th><th><\/th>';
		htmlstr += '<td colspan="'+(top.languages.length+6)+'">Or, <a href="javascript:" title="Associate this cognate set with the correspondence set that is currently selected (in the correspondence window)" onclick="newAssociation('+row+')">associate this set with the selected correspondence set.<\/a><\/td>';
		htmlstr += '<\/tr>';

		// reconstruction ("Proto")
		if(top.cognateProtoOpen[row])
			htmlstr += '<tr id="proto'+row+'" class="utility"><th><\/th><th><\/th>';
		else
			htmlstr += '<tr id="proto'+row+'" class="utility" style="display: none;"><th><\/th><th><\/th>';
		for(j=0;j<top.languages.length;j++)
		{
			if(top.col_visible[top.col_order[j]])
			{
				if(top.languages[top.col_order[j]].reconstructed==1)
				{
					str="";
					if(top.protoforms[row]!=null && top.protoforms[row][top.languages[top.col_order[j]].id]!=null)
					{
						str = top.protoforms[row][top.languages[top.col_order[j]].id];
					}
					htmlstr += '<td><input type="text" class="filltd" id="protoinput'+row+','+top.languages[top.col_order[j]].id+'" value="' + str + '" class="singlesegmentinput"\/><\/td>';
				}
				else
				{
					htmlstr += '<td><\/td>';
				}
			}
		}
		htmlstr += '<td colspan="8"><a href="javascript:" title="Save this proto-form" onclick="saveProto(\''+row+'\');">save<\/a><\/td>';
		htmlstr += '<\/tr>';

		// note
		if(top.cognateNoteOpen[row])
			htmlstr += '<tr id="note'+row+'" class="utility"><th><\/th><th><\/th>';
		else
			htmlstr += '<tr id="note'+row+'" class="utility" style="display: none;"><th><\/th><th><\/th>';
		htmlstr += '<td colspan="'+nvislanguages+'"><textarea id="notetext'+row+'" style="width: 100%;">'+top.cognate_notes[row]+'<\/textarea><\/td>';
		htmlstr += '<td colspan="8"><a href="javascript:" title="Save this note" onclick="saveNote('+row+');">save<\/a><\/td>';
		htmlstr += '<\/tr>';

	}
	htmlstr += '<\/table>';
	top.cognateFrame.contentDocument.getElementById("dataTable").innerHTML = htmlstr;
}

drawCorrespondenceTable = function drawCorrespondenceTable()
{
	if(top.correspondenceFrame==null || top.correspondenceFrame.contentDocument==null || top.correspondenceFrame.contentDocument.getElementById("dataTable")==null) { return; }
	
	reconstructedOptions="";
	for(i=0;i<top.languages.length;i++)
	{
		if(top.languages[i].reconstructed)
		{
			reconstructedOptions += '<option value="'+top.languages[i].id+'">'+top.languages[i].fullname+'<\/option>';
		}
	}

	htmlstr="";

	if(top.correspondences.length == 0)
	{
		top.correspondenceFrame.contentDocument.getElementById("dataTable").innerHTML = "<p><em>No correspondence sets defined so far.<\/em><\/p>";
		return;
	}

	if(top.col_order.length!=top.languages.length || top.col_visible.length!=top.languages.length)
	{
		top.initializeOrderVisibility();
	}

	htmlstr += '<table class="data_table">';
	if(!top.useAbbreviations)
	{
		nvislanguages=0;
		htmlstr += '<tr><th><\/th>';
		for(j=0;j<top.languages.length;j++)
		{
//			if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
			if(top.col_visible[top.col_order[j]])
			{
				htmlstr += '<th>'+top.languages[top.col_order[j]].fullname+'<\/th>';
				nvislanguages++;
			}
		}
	//	htmlstr += '<th><\/th><th><\/th><th style="head" colspan="2">Associated Cognate Sets<\/th>';
		htmlstr += '<\/tr>';
	}
	else
	{
		nvislanguages=0;
		htmlstr += '<tr id="abbr"><th><\/th>';
		for(j=0;j<top.languages.length;j++)
		{
//			if(top.col_visible[top.col_order[j]] && !top.languages[top.col_order[j]].reconstructed)
			if(top.col_visible[top.col_order[j]])
			{
				htmlstr += '<th>'+top.languages[top.col_order[j]].abbr+'<\/th>';
				nvislanguages++;
			}
		}
		htmlstr += '<\/tr>';
	}

	htmlstr += '<tr><th><\/th>';
	for(j=0;j<top.languages.length;j++)
	{
		if(top.col_visible[top.col_order[j]])
		{
			htmlstr += '<td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortCorrespondenceSets('+(languages[top.col_order[j]].id)+'); top.drawCorrespondenceTable();">sort<\/a><\/td>';
		}
	}
	htmlstr += '<\/tr>';

	if(top.correspondenceEditOpen.length!=top.correspondences.length || top.correspondenceReconOpen.length!=top.correspondences.length || top.correspondenceNoteOpen.length!=top.correspondences.length)
	{
		top.initializeCorrespondenceHiddenRows();
	}

	if(top.invalidateCorrespondenceSort)
	{
		top.invalidateCorrespondenceSort = 0;
		sortCorrespondenceSets(top.correspondenceSort);
	}

	for(var i in top.correspondenceSetRowOrder)
	{
		row = top.correspondenceSetRowOrder[i];
		stylestring = "";
		

		if(top.correspondenceFrameHighlights.indexOf(1*row) != -1)
		{
			if(top.correspondenceFrameSelectedRow == row)
			{
				stylestring = "highlighted_selected";
			}
			else
			{
				stylestring = "highlighted";
			}
		}
		else if(top.correspondenceFrameSelectedRow == row)
		{
			stylestring = "selected";
		}
		htmlstr += '<tr class="'+stylestring+'">';

		if(top.correspondenceFrameSelectedRow == row)
		{
			htmlstr += '<th><div id="scroll'+row+'"><\/div><input type="checkbox" checked="checked" onchange="rowselect('+row+')"><\/th>';
		}
		else
		{
			htmlstr += '<th><div id="scroll'+row+'"><\/div><input type="checkbox" onchange="rowselect('+row+')"><\/th>';
		}

		if(top.correspondenceSetRowOrder.length == 0)
		{
			sortCorrespondenceSets(null);
		}

		for(j=0;j<top.languages.length;j++)
		{
			if(top.col_visible[top.col_order[j]])
			{
				if(top.correspondences[row][top.languages[top.col_order[j]].id] != null)
				{
					string = top.correspondences[row][top.languages[top.col_order[j]].id];
				}
				else
				{
					string = "";
				}
				if(top.languages[top.col_order[j]].color.length > 0)
				{
					color = top.languages[top.col_order[j]].color;
				}
				else
				{
					color = "default";
				}
				htmlstr += '<td style="background-color: '+color+';" class="plaincursor" id="'+top.languages[j].id+','+top.col_order[j]+'">'+string+'<\/td>';
			}
		}

		if(top.correspondenceEditOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit this correspondence set" onclick="toggleEditForm('+row+')" id="edit'+row+'">'+strEditAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit this correspondence set" onclick="toggleEditForm('+row+')" id="edit'+row+'">'+strEditBefore+'<\/a><\/td>';
		if(top.correspondenceReconOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to reconstruct a segment" onclick="toggleCorrespondenceForm('+row+')" id="link'+row+'">'+strReconAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to reconstruct a segment" onclick="toggleCorrespondenceForm('+row+')" id="link'+row+'">'+strReconBefore+'<\/a><\/td>';
		if(top.correspondenceChangeOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to create a sound change" onclick="toggleChangeForm('+row+')" id="chlink'+row+'">'+strChangeAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to create a sound change" onclick="toggleChangeForm('+row+')" id="chlink'+row+'">'+strChangeBefore+'<\/a><\/td>';
		if(top.correspondenceNoteOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to write a note" onclick="toggleNoteForm('+row+')" id="nlink'+row+'">'+strShowNoteAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to write a note" onclick="toggleNoteForm('+row+')" id="nlink'+row+'">'+strShowNoteBefore+'<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the cognate sets associated with this correspondence set" onclick="top.highlightCognatesFromCorrespondence('+row+')">'+strHighlightCognates+' ('+top.correspondence_cognate_counts[row]+')<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the reconstructions associated with this correspondence set" onclick="top.highlightReconstructionsFromCorrespondence('+row+')">'+strHighlightReconstructions+' ('+top.correspondence_reconstruction_counts[row]+')<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Disassociate the selected cognate sets from this correspondence set" onclick="removeSelected('+row+')">'+strRemoveSelected+'<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Remove this correspondence set" onclick="removeCorrespondence('+row+')">X<\/a><\/td>';
		htmlstr += '<\/tr>';

		// edit
		if(top.correspondenceEditOpen[row])
		{
			htmlstr += '<tr id="editrow'+row+'" class="utility"><th><\/th>';
		}
		else
		{
			htmlstr += '<tr id="editrow'+row+'" class="utility" style="display: none;"><th><\/th>';
		}
		for(j=0;j<top.languages.length;j++)
		{
			if(top.col_visible[top.col_order[j]])
			{
				if(top.correspondences[row][top.languages[top.col_order[j]].id] != null)
				{
					string = top.correspondences[row][top.languages[top.col_order[j]].id];
				}
				else
				{
					string = "";
				}
				htmlstr += '<td><input type="text" id="c'+row+','+top.languages[top.col_order[j]].id+'" value="'+string+'" class="singlesegmentinput"\/><\/td>';
			}
		}
		htmlstr += '<td colspan="5"><a href="javascript:" title="Save the changes you have entered" onclick="editCorrespondence('+row+')">Save<\/a><\/td>';
		htmlstr += '<\/tr>';


		// reconstruct
		if(top.correspondenceReconOpen[row])
			htmlstr += '<tr id="corr'+row+'" class="utility"><th><\/th>';
		else
			htmlstr += '<tr id="corr'+row+'" class="utility" style="display: none;"><th><\/th>';
		if(reconstructedOptions.length>0)
		{
			htmlstr += '<td colspan="'+(nvislanguages+6)+'"><form method="get"><label>Reconstruct <\/label><input type="text" size="1" id="recon'+row+'" value="" class="singlesegmentinput"\/><label> in <\/label><select id="proto'+row+'">'+reconstructedOptions+'<\/select> <input type="button" value="Add Reconstruction" onclick="reconstruct('+row+')"\/><\/form><\/td>';
		}
		else
		{
			htmlstr += '<td colspan="'+(nvislanguages+6)+'"><em>Create a proto-language in order to be able to posit reconstructions.<\/em><\/td>';
		}
		htmlstr += '<\/tr>';
		if(top.correspondenceReconOpen[row])
			htmlstr += '<tr id="excorr'+row+'" class="utility"><th><\/th>';
		else
			htmlstr += '<tr id="excorr'+row+'" class="utility" style="display: none;"><th><\/th>';
		htmlstr += '<td colspan="'+(nvislanguages+6)+'">Or, <a href="javascript:" title="Associate this correspondence set with the currently selected reconstruction (in the reconstruction window)" onclick="newAssociation('+row+')">associate this set with the selected reconstruction.<\/a><\/td>';
		htmlstr += '<\/tr>';

		// sound change
		if(top.correspondence_reconstruction_counts[row] > 0)
		{
		if(top.correspondenceChangeOpen[i])
		{
			htmlstr += '<tr id="cheditrow'+row+'" class="utility"><th><\/th><td colspan="11"><table><tr>';
		}
		else
		{
			htmlstr += '<tr id="cheditrow'+row+'" class="utility" style="display: none;"><th><\/th><td colspan="11"><table><tr>';
		}

		htmlstr += '<td><select id="chedit_reconstruction'+row+'">'+reconstructionOptionsFromCorrespondence(row)+'<\/select><\/td>';
		htmlstr += "<td>&gt;<\/td>";

		htmlstr += '<td><input type="text" id="chedit_becomes'+row+'" value="" class="singlesegmentinput"\/><\/td>';
		htmlstr += "<td>/<\/td>";
		htmlstr += '<td style="text-align: center;"><input type="text" id="chedit_before'+row+'" value="" class="singlesegmentinput"\/><\/td>';
		htmlstr += "<td>___<\/td>";
		htmlstr += '<td style="text-align: center;"><input type="text" id="chedit_after'+row+'" value="" class="singlesegmentinput"\/><\/td>';
		htmlstr += "<td>in<\/td>";

		htmlstr += '<td><select id="chedit_inlanguage'+row+'" onchange="top.updateRuleSelectForCorrespondences(\'chedit_follows'+row+'\',\'chedit_reconstruction'+row+'\',\'chedit_inlanguage'+row+'\',-1,-1)">'+languageOptions(-1)+'<\/select><\/td>';
		htmlstr += "<td>Applies after:<\/td>";
		htmlstr += '<td><select id="chedit_follows'+row+'"><\/td>';

		htmlstr += '<td><a href="#" onclick="addSoundChange('+row+')">save<\/a><\/td>';

		htmlstr += '<\/tr><\/table><\/td><\/tr>';
		}
		else {
			if(top.correspondenceChangeOpen[i])
			{
				htmlstr += '<tr id="cheditrow'+row+'" class="utility"><th><\/th><td colspan="11"><em>You need to reconstruct a segment before you create a sound change.<\/em><\/td><\/tr>';
			}
			else
			{
				htmlstr += '<tr id="cheditrow'+row+'" class="utility" style="display: none;"><th><\/th><td colspan="11"><em>You need to reconstruct a segment before you create a sound change.<\/em><\/td><\/tr>';
			}
		}

		// note
		if(top.correspondenceNoteOpen[row])
			htmlstr += '<tr id="note'+row+'" class="utility"><th><\/th><th><\/th>';
		else
			htmlstr += '<tr id="note'+row+'" class="utility" style="display: none;"><th><\/th>';
		htmlstr += '<td colspan="'+nvislanguages+'"><textarea id="notetext'+row+'" style="width: 100%;">'+top.correspondence_notes[row]+'<\/textarea><\/td>';
		htmlstr += '<td colspan="8"><a href="javascript:" title="Save this note" onclick="top.app.saveCorrespondenceNote('+row+');">save<\/a><\/td>';
		htmlstr += '<\/tr>';
	}
	htmlstr += '<\/table>';
	top.correspondenceFrame.contentDocument.getElementById("dataTable").innerHTML = htmlstr;
	
	// have to update these once the new DOM bits are actually in place
	for(i in top.correspondenceSetRowOrder)
	{
		row = top.correspondenceSetRowOrder[i];
		id = 'chedit_follows'+row;
		from = 'chedit_reconstruction'+row;
		to = 'chedit_inlanguage'+row;
		top.updateRuleSelectForCorrespondences(id,from,to,-1,-1);
	}
}

drawReconstructionTable = function drawReconstructionTable()
{
	if(top.reconstructionFrame==null || top.reconstructionFrame.contentDocument==null || top.reconstructionFrame.contentDocument.getElementById("dataTable")==null) { return; }

	htmlstr="";

	if(top.reconstructions.length == 0)
	{
		top.reconstructionFrame.contentDocument.getElementById("dataTable").innerHTML = "<p><em>No reconstructions defined so far.<\/em><\/p>";
		return;
	}

	if(top.reconstructionNoteOpen.length!=top.reconstructions.length)
	{
		top.initializeReconstructionHiddenRows();
	}

	if(top.invalidateReconstructionSort)
	{
		top.invalidateReconstructionSort = 0;
		sortReconstructions(top.reconstructionSort);
	}

		sortReconstructions(top.reconstructionSort);

	htmlstr += '<table class="data_table">';

	htmlstr += '<tr><th><\/th><th>Language<\/th><th>Reconstruction<\/th><th># Corr<\/th><th># Cog<\/th><th># Data<\/th><th># Lang<\/th><\/tr>';

	htmlstr += '<tr><th><\/th><td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortReconstructions(\'language\'); top.drawReconstructionTable();">sort<\/a><\/td><td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortReconstructions(\'reconstruction\'); top.drawReconstructionTable();">sort<\/a><\/td><td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortReconstructions(\'ncor\'); top.drawReconstructionTable();">sort<\/a><\/td><td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortReconstructions(\'ncog\'); top.drawReconstructionTable();">sort<\/a><\/td>';
	htmlstr += '<td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortReconstructions(\'data\'); top.drawReconstructionTable();">sort<\/a><\/td><td class="colutility"><a href="javascript:" title="Sort rows by the values in this column" onclick="top.sortReconstructions(\'languages\'); top.drawReconstructionTable();">sort<\/a><\/td>';
	htmlstr += '<\/tr>';
	
	for(var i in top.reconstructionRowOrder)
	{
		row = top.reconstructionRowOrder[i];
		stylestring="";
		if(top.reconstructionFrameHighlights.indexOf(top.reconstructions[row].id) != -1)
		{
			if(top.reconstructionFrameSelectedRow == top.reconstructions[row].id)
			{
				stylestring = 'highlighted_selected';
			}
			else
			{
				stylestring = 'highlighted';
			}
		}
		else if(top.reconstructionFrameSelectedRow == top.reconstructions[row].id) {
				stylestring = 'selected';		
		}

		if(top.reconstructionFrameSelectedRow == top.reconstructions[row].id)
		{
			htmlstr += '<tr class="'+stylestring+'"><th><div id="scroll'+top.reconstructions[row].id+'"><\/div><input type="checkbox" checked="checked" onchange="rowselect('+top.reconstructions[row].id+')"><\/th>';
		}
		else
		{
			htmlstr += '<tr class="'+stylestring+'"><th><div id="scroll'+top.reconstructions[row].id+'"><\/div><input type="checkbox" onchange="rowselect('+top.reconstructions[row].id+')"><\/th>';
		}
		if(top.reconstructions[row].color.length > 0)
		{
			color = top.reconstructions[row].color;
		}
		else
		{
			color = "default";
		}

		htmlstr += '<td class="plaincursor" style="background-color: '+color+';">'+top.reconstructions[row].fullname+'<\/td>';
		htmlstr += '<td class="plaincursor">'+top.reconstructions[row].string+'<\/td>';
		htmlstr += '<td class="plaincursor">'+1*top.recon_correspondences[top.reconstructions[row].id]+'<\/td>';
		htmlstr += '<td class="plaincursor">'+1*top.recon_cognates[top.reconstructions[row].id]+'<\/td>';
		htmlstr += '<td class="plaincursor">'+1*top.recon_items[top.reconstructions[row].id]+'<\/td>';
		htmlstr += '<td class="plaincursor">'+1*top.recon_languages[top.reconstructions[row].id]+'<\/td>';


		if(top.reconstructionEditOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit the reconstruction" onclick="toggleEditForm('+row+')" id="editlink'+row+'">'+strShowEditAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit the reconstruction" onclick="toggleEditForm('+row+')" id="editlink'+row+'">'+strShowEditBefore+'<\/a><\/td>';
		if(top.reconstructionNoteOpen[row])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to write a note" onclick="toggleNoteForm('+row+')" id="nlink'+row+'">'+strShowNoteAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to write a note" onclick="toggleNoteForm('+row+')" id="nlink'+row+'">'+strShowNoteBefore+'<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the correspondence sets associated with this reconstruction" onclick="highlightCorrespondences('+top.reconstructions[row].id+')">'+strViewCorrespondences+' ('+1*top.recon_correspondences[top.reconstructions[row].id]+')<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the correspondence sets and cognate sets associated with this reconstruction" onclick="highlightCorrespondencesCognates('+top.reconstructions[row].id+')">'+strPlusCognates+' ('+1*top.recon_cognates[top.reconstructions[row].id]+')<\/a><\/td>';


		htmlstr += '<td class="button"><a href="javascript:" title="Disassociate from this reconstruction the currently selected correspondence set (in the correspondence window)" onclick="removeSelected('+top.reconstructions[row].id+');">'+strReconRemoveSelected+'<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Remove this reconstruction" onclick="removeReconstruction('+top.reconstructions[row].id+');">X<\/a><\/td>';

		htmlstr += '<\/tr>';

		// edit
		if(top.reconstructionEditOpen[row])
		{
			htmlstr += '<tr id="editrow'+row+'" class="utility"><th><\/th>';
		}
		else
		{
			htmlstr += '<tr id="editrow'+row+'" class="utility" style="display: none;"><th><\/th>';
		}
		htmlstr += '<td><\/td>';
		htmlstr += '<td><input type="text" id="editfield'+top.reconstructions[row].id+'" value="'+top.reconstructions[row].string+'" class="singlesegmentinput"\/><\/td>';
		htmlstr += '<td colspan="4"><a href="javascript:" title="Save the change to the reconstruction" onclick="editReconstruction('+top.reconstructions[row].id+')">Save<\/a><\/td>';
		htmlstr += '<\/tr>';

		// note
		if(top.correspondenceNoteOpen[row])
			htmlstr += '<tr id="note'+row+'" class="utility"><th><\/th><th><\/th>';
		else
			htmlstr += '<tr id="note'+row+'" class="utility" style="display: none;"><th><\/th>';
		htmlstr += '<td colspan="4"><textarea id="notetext'+top.reconstructions[row].id+'" style="width: 100%;">'+top.reconstruction_notes[top.reconstructions[row].id]+'<\/textarea><\/td>';
		htmlstr += '<td colspan="3"><a href="javascript:" title="Save this note" onclick="saveNote('+top.reconstructions[row].id+');">save<\/a><\/td>';
		htmlstr += '<\/tr>';

	}
	htmlstr += '<\/table>';
	top.reconstructionFrame.contentDocument.getElementById("dataTable").innerHTML = htmlstr;

	// similar kinds of things need to trigger redraws, so call this also
	top.drawSoundChangeTable();
}

drawSoundChangeTable = function drawSoundChangeTable()
{
	if(top.reconstructionFrame==null || top.reconstructionFrame.contentDocument==null || top.reconstructionFrame.contentDocument.getElementById("soundChangeTable")==null) { return; }

	// update the "add sound change" fields at this time
/*
	top.reconstructionFrame.contentDocument.getElementById("fld_reconstruction").innerHTML = reconstructionOptions(-1);
	top.reconstructionFrame.contentDocument.getElementById("fld_inlanguage").innerHTML = languageOptions(-1);
	top.updateRuleSelect('fld_follows','fld_reconstruction','fld_inlanguage',-1,-1);
*/
	if(top.soundchanges.length == 0)
	{
		top.reconstructionFrame.contentDocument.getElementById("soundChangeTable").innerHTML = "<p><em>No sound changes defined so far.<\/em><\/p>";
		return;
	}


	htmlstr="";
	htmlstr += '<table class="data_table">';
	htmlstr += '<tr><th>Language<\/th><th>Reconstruction<\/th><th><\/th><th>Becomes<\/th><th><\/th><th>Before<\/th><th><\/th><th>After<\/th><th><\/th><th>In Language<\/th><th>Follows<\/th><\/tr>';

	var j;
	for(var i in top.soundchanges)
	{
		htmlstr += "<tr>";
		htmlstr += '<td style="text-align: center;">' + top.soundchanges[i].fullname + "<\/td>";
		htmlstr += '<td style="text-align: center;">' + top.soundchanges[i].string + "<\/td>";
		htmlstr += "<td>&gt;<\/td>";
		htmlstr += '<td style="text-align: center;">' + top.soundchanges[i].becomes + "<\/td>";
		htmlstr += "<td>/<\/td>";
		htmlstr += '<td style="text-align: center;">' + top.soundchanges[i].before + "<\/td>";
		htmlstr += "<td>___<\/td>";
		htmlstr += '<td style="text-align: center;">' + top.soundchanges[i].after + "<\/td>";
		htmlstr += "<td>in<\/td>";
		for(j in top.languages) { if(top.languages[j].id==top.soundchanges[i].inlanguage) { htmlstr += "<td>" + top.languages[j].fullname + "<\/td>"; } }

		thestr = "";
		for(j in top.soundchanges)
		{
			if( top.soundchanges[i].follows == top.soundchanges[j].id )
				thestr = top.soundchanges[j].string + '>' + top.soundchanges[j].becomes + '/' + top.soundchanges[j].before + '__' + top.soundchanges[j].after;
		}
		htmlstr += '<td>' + thestr + '<\/td>';

		if(top.soundchangeEditOpen[i])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit this sound change" onclick="toggleSCEditForm('+i+')" id="scedit'+i+'">'+strEditAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to edit this sound change" onclick="toggleSCEditForm('+i+')" id="scedit'+i+'">'+strEditBefore+'<\/a><\/td>';
		if(top.soundchangeNoteOpen[i])
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to write a note" onclick="toggleSCNoteForm('+i+')" id="scnlink'+i+'">'+strShowNoteAfter+'<\/a><\/td>';
		else
			htmlstr += '<td class="button dropdownbutton"><a href="javascript:" title="Show/hide a form to write a note" onclick="toggleSCNoteForm('+i+')" id="scnlink'+i+'">'+strShowNoteBefore+'<\/a><\/td>';
		htmlstr += '<td class="button crossreferencebutton"><a href="javascript:" title="Highlight the cognate sets & protoforms associated with this reconstruction" onclick="highlightProtoforms('+top.soundchanges[i].id+')">'+strViewProtoform+' ('+soundchange_cognate_counts[top.soundchanges[i].id]+')<\/a><\/td>';

		htmlstr += '<td class="button"><a href="javascript:" title="Associate this sound change with the currently selected cognate set\/protoform (in the cognate set window)" onclick="associateProtoform('+top.soundchanges[i].id+');">'+strSCAssoc+'<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Disassociate this sound change from the currently selected cognate set\/protoform (in the cognate set window)" onclick="disassociateProtoform('+top.soundchanges[i].id+');">'+strSCDisassoc+'<\/a><\/td>';
		htmlstr += '<td class="button"><a href="javascript:" title="Remove this sound change" onclick="removeSoundChange('+top.soundchanges[i].id+')">X<\/a><\/td>';

		htmlstr += "<\/tr>";

		// edit
		if(top.soundchangeEditOpen[i])
		{
			htmlstr += '<tr id="sceditrow'+i+'" class="utility"><td colspan="11"><table><tr>';
		}
		else
		{
			htmlstr += '<tr id="sceditrow'+i+'" class="utility" style="display: none;"><td colspan="11"><table><tr>';
		}
		htmlstr += '<td><select id="scedit_reconstruction'+top.soundchanges[i].id+'">'+reconstructionOptions(top.soundchanges[i].reconstruction)+'<\/select><\/td>'; /// HACK
		htmlstr += "<td>&gt;<\/td>";
		htmlstr += '<td><input type="text" id="scedit_becomes'+top.soundchanges[i].id+'" value="'+top.soundchanges[i].becomes+'" class="singlesegmentinput"\/><\/td>';
		htmlstr += "<td>/<\/td>";
		htmlstr += '<td style="text-align: center;"><input type="text" id="scedit_before'+top.soundchanges[i].id+'" value="'+top.soundchanges[i].before+'" class="singlesegmentinput"\/><\/td>';
		htmlstr += "<td>___<\/td>";
		htmlstr += '<td style="text-align: center;"><input type="text" id="scedit_after'+top.soundchanges[i].id+'" value="'+top.soundchanges[i].after+'" class="singlesegmentinput"\/><\/td>';
		htmlstr += "<td>in<\/td>";
		htmlstr += '<td><select id="scedit_inlanguage'+top.soundchanges[i].id+'">'+languageOptions(top.soundchanges[i].inlanguage)+'<\/select><\/td>';
		htmlstr += "<td>After:<\/td>";
		htmlstr += '<td><select id="scedit_follows'+top.soundchanges[i].id+'"><\/select><\/td>';

		htmlstr += '<td><a href="#" onclick="soundchangeEdit('+top.soundchanges[i].id+')">save<\/a><\/td>';

		htmlstr += '<\/tr><\/table><\/td><\/tr>';

		// note
		if(top.soundchangeNoteOpen[i])
			htmlstr += '<tr id="scnote'+i+'" class="utility">';
		else
			htmlstr += '<tr id="scnote'+i+'" class="utility" style="display: none;">';
		htmlstr += '<td colspan="4"><textarea id="scnotetext'+top.soundchanges[i].id+'" style="width: 100%;">'+top.soundchange_notes[top.soundchanges[i].id]+'<\/textarea><\/td>';
		htmlstr += '<td colspan="3"><a href="javascript:" title="Save this note" onclick="saveSCNote('+top.soundchanges[i].id+');">save<\/a><\/td>';
		htmlstr += '<\/tr>';

	}

	htmlstr += '<\/table>';
	top.reconstructionFrame.contentDocument.getElementById("soundChangeTable").innerHTML = htmlstr;

	// have to update these once the new DOM bits are actually in place
	for(var i in top.soundchanges)
		top.updateRuleSelect('scedit_follows'+top.soundchanges[i].id,'scedit_reconstruction'+top.soundchanges[i].id,'scedit_inlanguage'+top.soundchanges[i].id,top.soundchanges[i].follows,top.soundchanges[i].id);
}

languageOptions = function languageOptions(selected)
{
	var thestr = '<option value="-1">--None--<\/option>';
	for(var i in top.languages)
	{
		if(top.languages[i].id==selected)
			thestr += '<option selected="selected" value="'+top.languages[i].id+'">' + top.languages[i].fullname + '<\/option>';
		else
			thestr += '<option value="'+top.languages[i].id+'">' + top.languages[i].fullname + '<\/option>';
	}
	return thestr;
}

reconstructionOptionsFromCorrespondence = function reconstructionOptionsFromCorrespondence(correspondence)
{
	var thestr = "";

	for(var i in top.reconstructions)
	{
		if( top.cor_recon_dep[top.reconstructions[i].id] != null && top.cor_recon_dep[top.reconstructions[i].id][correspondence] != null)
			thestr += '<option value="'+top.reconstructions[i].id+'">' + top.reconstructions[i].string + '<\/option>';
	}
	return thestr;
}

reconstructionOptions = function reconstructionOptions(selected)
{
	var thestr = '<option value="-1">--None--<\/option>';
	previous = "notalikelynameforalanguage";
	first = 1;
	for(var i in top.reconstructions)
	{
		if(top.reconstructions[i].fullname != previous)
		{
			if(!first) { thestr += "<\/optgroup>"; }
			first = 0;
			thestr += '<optgroup label="'+top.reconstructions[i].fullname+'">';
			previous = top.reconstructions[i].fullname;
		}
		if(top.reconstructions[i].id == selected)
			thestr += '<option selected="selected" value="'+top.reconstructions[i].id+'">' + top.reconstructions[i].string + '<\/option>';
		else
			thestr += '<option value="'+top.reconstructions[i].id+'">' + top.reconstructions[i].string + '<\/option>';
	}
	thestr += "<\/optgroup>";
	return thestr;
}

updateRuleSelect = function updateRuleSelect(id,from,to,selected,not)
{
	var i;
	var thestr = "";
	thestr += '<option value="-1"> --None--<\/option>';
	
	for(i in top.reconstructions)
	{
		if(top.reconstructions[i].id == top.reconstructionFrame.contentDocument.getElementById(from).value) { from = top.reconstructions[i].langid; break; }
	}

	for(i in top.soundchanges)
	{
		if( not!=top.soundchanges[i].id && from==top.soundchanges[i].sourcelanguage && top.reconstructionFrame.contentDocument.getElementById(to).value==top.soundchanges[i].inlanguage )
		{
			if(top.soundchanges[i].id == selected)
				thestr += '<option selected="selected" value="'+top.soundchanges[i].id+'">' + top.soundchanges[i].string + '>' + top.soundchanges[i].becomes + '/' + top.soundchanges[i].before + '__' + top.soundchanges[i].after + '<\/option>';
			else
				thestr += '<option value="'+top.soundchanges[i].id+'">' + top.soundchanges[i].string + '>' + top.soundchanges[i].becomes + '/' + top.soundchanges[i].before + '__' + top.soundchanges[i].after + '<\/option>';
		}
	}
	thestr += "<\/optgroup>";

	top.reconstructionFrame.contentDocument.getElementById(id).innerHTML = thestr;
}

updateRuleSelectForCorrespondences = function updateRuleSelectForCorrespondences(id,from,to,selected,not)
{
	var i;
	var thestr = "";
	thestr += '<option value="-1"> --None--<\/option>';

	if(top.correspondenceFrame.contentDocument.getElementById(from) == null)
	{
		return;
	}

	for(i in top.reconstructions)
	{
		if(top.reconstructions[i].id == top.correspondenceFrame.contentDocument.getElementById(from).value) { from = top.reconstructions[i].langid; break; }
	}

	for(i in top.soundchanges)
	{
		if( not!=top.soundchanges[i].id && from==top.soundchanges[i].sourcelanguage && top.correspondenceFrame.contentDocument.getElementById(to).value==top.soundchanges[i].inlanguage )
		{
			if(top.soundchanges[i].id == selected)
				thestr += '<option selected="selected" value="'+top.soundchanges[i].id+'">' + top.soundchanges[i].string + '>' + top.soundchanges[i].becomes + '/' + top.soundchanges[i].before + '__' + top.soundchanges[i].after + '<\/option>';
			else
				thestr += '<option value="'+top.soundchanges[i].id+'">' + top.soundchanges[i].string + '>' + top.soundchanges[i].becomes + '/' + top.soundchanges[i].before + '__' + top.soundchanges[i].after + '<\/option>';
		}
	}
	thestr += "<\/optgroup>";

	top.correspondenceFrame.contentDocument.getElementById(id).innerHTML = thestr;
}

followsOptions = function followsOptions()
{
	var thestr = "";
	for(var i in top.soundchanges)
	{
		thestr += '<option value="'+top.soundchanges[i].id+'">' + top.soundchanges[i].string + '>' + top.soundchanges[i].becomes + '/' + top.soundchanges[i].before + '__' + top.soundchanges[i].after + '<\/option>';
	}
	thestr += "<\/optgroup>";
	return thestr;
}

sortData = function sortData(langid)
{
	top.dataSort = langid;

	tarr = Array(top.items.length);
	starr = Array(top.items.length);
	var order = Array();

	if(top.dataSort==null) { top.dataSort='indexorder'; }

	if(top.dataSort=="indexorder")
	{
		var ct=0;
		for(var i in top.items)
		{
			order[ct] = 1*i;
			ct++;
		}
	}
	else if(top.dataSort!=null)
	{
		for(var i in top.items)
		{
			tarr[i] = top.items[i][top.dataSort];
			starr[i] = top.items[i][top.dataSort];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort();
		ct = 0;
		for(var i in starr)
		{
			if(starr[i]==null) { continue; }
			order[ct] = tarr.indexOf(starr[i]);
			ct++;
			tarr[order[i]] = null;
		}
	}
	else
	{
		for(var i in top.items)
		{
			tarr[i] = top.lemmas[i];
			starr[i] = top.lemmas[i];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort();
		ct = 0;
		for(var i in starr)
		{
			if(starr[i]==null) { continue; }
			order[ct] = tarr.indexOf(starr[i]);
			ct++;
			tarr[order[i]] = null;
		}
	}
	top.dataRowOrder = order;
}

sortCognateSets = function sortCognateSets(langid)
{
	top.cognateSort = langid;

	if(top.cognateSort==null) { top.cognateSort='indexorder'; }

	tarr = Array(top.cognate_labels.length);
	starr = Array(top.cognate_labels.length);
	var order = Array();

	if(top.cognateSort=="indexorder")
	{
		for(var i in top.cognate_labels)
		{
			order[i] = 1*i;
		}
//		order.trim();
	}
	else if(top.cognateSort!=null)
	{
		for(var i in top.cognate_labels)
		{
			tarr[i] = top.cognate_sets[i][top.cognateSort];
			starr[i] = top.cognate_sets[i][top.cognateSort];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort();
		ct = 0;
		for(var i in starr)
		{
			if(starr[i]==null) { continue; }
			order[ct] = tarr.indexOf(starr[i]);
			ct++;
			tarr[order[i]] = null;
		}
	}
	else
	{
		for(var i in top.cognate_labels)
		{
			tarr[i] = top.cognate_labels[i];
			starr[i] = top.cognate_labels[i];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort();
		ct = 0;
		for(var i in starr)
		{
			if(starr[i]==null) { continue; }
			order[ct] = tarr.indexOf(starr[i]);
			ct++;
			tarr[order[i]] = null;
		}
	}
	top.cognateSetRowOrder = order;
}

sortCorrespondenceSets = function sortCorrespondenceSets(langid)
{
	top.correspondenceSort = langid;

	if(top.correspondenceSort==null) { top.correspondenceSort='indexorder'; }

	tarr = Array(top.correspondences.length);
	starr = Array(top.correspondences.length);
	var order = Array();

	if(top.correspondenceSort=="indexorder")
	{
		for(var i in top.correspondences)
		{
			order[i] = 1*i;
		}
//		order.trim();
	}
	else if(top.correspondenceSort!=null)
	{
		for(var i in top.correspondences)
		{
			tarr[i] = top.correspondences[i][top.correspondenceSort];
			starr[i] = top.correspondences[i][top.correspondenceSort];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort();
		ct = 0;
		for(var i in starr)
		{
			if(starr[i]==null) { continue; }
			order[ct] = tarr.indexOf(starr[i]);
			ct++;
			tarr[order[i]] = null;
		}
	}
	else
	{
		ct=0;
		for(var i in top.correspondences)
		{
			order[ct] = i;
			ct++;
		}
	}
	top.correspondenceSetRowOrder = order;
}

sortReconstructions = function sortReconstructions(mode)
{
	top.reconstructionSort = mode;

	tarr = Array(top.reconstructions.length);
	starr = Array(top.reconstructions.length);
	var order = Array();

	if(top.reconstructionSort==null) { top.reconstructionSort='indexorder'; }

	switch(top.reconstructionSort)
	{
	case 'indexorder':
		for(var i in top.reconstructions)
		{
			order[i] = 1*i;
		}		
//		order.trim();
	case 'language':
		for(var i in top.reconstructions)
		{
			tarr[i] = top.reconstructions[i].fullname;
			starr[i] = top.reconstructions[i].fullname;
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort(function(x,y){ 
		      var a = String(x).toUpperCase(); 
		      var b = String(y).toUpperCase(); 
		      if (a > b) 
			 return 1 
		      if (a < b) 
			 return -1 
		      return 0; 
		    });
		break;
	case 'reconstruction':
		for(var i in top.reconstructions)
		{
			tarr[i] = top.reconstructions[i].string;
			starr[i] = top.reconstructions[i].string;
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort(function(x,y){ 
		      var a = String(x).toUpperCase(); 
		      var b = String(y).toUpperCase(); 
		      if (a > b) 
			 return 1 
		      if (a < b) 
			 return -1 
		      return 0; 
		    });
		break;
	case 'ncor':
		for(var i in top.reconstructions)
		{
			tarr[i] = 1*top.recon_correspondences[top.reconstructions[i].id];
			starr[i] = 1*top.recon_correspondences[top.reconstructions[i].id];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort(function(x,y){ return y-x; });
		break;
	case 'ncog':
		for(var i in top.reconstructions)
		{
			tarr[i] = 1*top.recon_cognates[top.reconstructions[i].id];
			starr[i] = 1*top.recon_cognates[top.reconstructions[i].id];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort(function(x,y){ return y-x; });
		break;
	case 'data':
		for(var i in top.reconstructions)
		{
			tarr[i] = 1*top.recon_items[top.reconstructions[i].id];
			starr[i] = 1*top.recon_items[top.reconstructions[i].id];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort(function(x,y){ return y-x; });
		break;
	case 'languages':
		for(var i in top.reconstructions)
		{
			tarr[i] = 1*top.recon_languages[top.reconstructions[i].id];
			starr[i] = 1*top.recon_languages[top.reconstructions[i].id];
			if(tarr[i]==null)
			{
				tarr[i] = '@';
				starr[i] = '@';
			}
		}
		starr.sort(function(x,y){ return y-x; });
		break;
	}

	ct = 0;
	for(var i in starr)
	{
		if(starr[i]==null) { continue; }
		order[ct] = tarr.indexOf(starr[i]);
		ct++;
		tarr[order[i]] = null;
	}
	
	top.reconstructionRowOrder = order;
}
