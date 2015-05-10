
function globals()
        for n,v in pairs(_G) do
                print(n)
                end
        end


function print_vector( v )
	s = ""
	for i = 0, 3, 1 do
		s = s .. v:Get( i ) .. " "
	end
	print( s )
end

function print_matrix( m )

	for i = 0, 3, 1 do
		s = ""
		for j = 0, 3, 1 do
			s = s .. m:Get( i, j ) .. " "
		end
		print( s )
	end
end

function on_scenegraph_event( evt_type, scenegraph_name, node_alias )

  local evt_type_name
  
  if evt_type == 0 then
    evt_type_name = "NODE_ADDED"
  end

  print( "scenegraph event -> " .. node_alias .. " " .. evt_type_name .. " on " .. scenegraph_name )
  
end

print( "Lua version : " .. _VERSION )

ds = DrawSpace()