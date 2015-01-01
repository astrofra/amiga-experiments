-------------------------------------------
-- spline2json.ms
-- exports a set of wireframe objects (spline) to an json file
-- initially based on a Maxscript by https://github.com/scorpheus
-- Version: 1.0
-------------------------------------------

global theFileStream

fn round_to val n =
(
	local mult = 10.0 ^ n
	(floor ((val * mult) + 0.5)) / mult
)

fn printToFile theString t:0 = 
(
  -- auto tab (empty string)
  tabString = " ";
  tabString = substring tabString 1 (t*4);
  -- print
  format "%%\n" tabString theString to:theFileStream;
)


----------------------- Settings Window -----------------------
rollout Rollo "Export Spline Object" width:400 height:400
(
	global mystring
	
----------------------- Main Menu -----------------------

	label espace1 height:3
	Button LaunchExportPathButton "Export splines as nut" width:140 height:25 align:#center offset:[7,0]
	label espace2 height:3
	
	
	-- Export paths
	on LaunchExportPathButton pressed do
	(
		fileName = getSaveFileName caption:"Save A File:" filename:("export.json")
		
		if fileName != undefined then 
		(			
			theFileStream = createFile fileName;

			-- ok to write?
			if theFileStream != undefined then   
			(					  
				-- root node
				printToFile ("{")
				-- printToFile ("//	Object vector description")
				printToFile ("	\"object_vectors\" :	{")
				
				-- create the knot item
				
				j = 0
				for s in Objects where classOf s == SplineShape do
				( 
					-- test if the spline contains spline and knot
					if (numSplines s) > 0 and (numKnots s 1) > 0 do
					(
						k     = numKnots s       -- if crash here, it's because the spline contains sub-spline, and that's not good.											
						spline_name = s.name

						printToFile ("		\"object\" : {	\"object_name\" : \"" + (spline_name as String) + "\",")
						-- printToFile ("		//	max_knots = " + (k as String) + ",")

						segment_index = 1
						total_number_of_segs = 0

						printToFile ("			\"segments\" : [")

						do
						(	
							number_of_segs = numKnots s segment_index

							-- printToFile ("				// Current segments contains " + (number_of_segs as String) + " knots.")
							printToFile ("					[")	
							-- knot data
							for i = 1 to number_of_segs do
							(
								knot = getKnotPoint s segment_index i
								x    = (round_to (knot.x - s.pos.x) 2) as String
								y    = (round_to (knot.z - s.pos.z) 2) as String							
								z    = (round_to (knot.y - s.pos.y) 2) as String

								if i < number_of_segs then
									printToFile ("							[" + x + "," + y + "," + z + "],")
								else
									printToFile ("							[" + x + "," + y + "," + z + "]")
							)

							if segment_index < numSplines s then
								printToFile ("					],")
							else
								printToFile ("					]")								
							
							segment_index = segment_index + 1
							total_number_of_segs = total_number_of_segs + number_of_segs
						)
						while segment_index <= numSplines s
						
						printToFile ("				]")	

						printToFile	("		}")					
					)
				)								
				printToFile ("	}")
				printToFile ("}")
			)
			
		close theFileStream;
		)
	)
)

----------------------- About -----------------------
rollout a_propos "About" width:600 height:600
(
	label espace18 height:3
	label lbl1 "Spline to Json Exporter" align:#center
	label lbl2 "Version :  1.0" align:#center
	label espace19 height:3
	label lbl3 "--- astrofra.com ---" align:#center
	label lbl4 "Based on an export" align:#center
	label lbl5 "script by Scorpheus" align:#center
	label espace20 height:3
)


script_window = newRolloutFloater "MaxSpline2JSONExport v1.0" 330 250
addRollout Rollo script_window
addRollout a_propos script_window rolledup:false