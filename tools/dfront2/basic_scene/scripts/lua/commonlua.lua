
collisions_transforms = {}
collisions_chunks = {}
collisions_pass = {}

create_collision_meshes = false

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

function create_collision_chunk( shape_type, collision_chunk_node_alias, collision_transform_node_alias, scenegraph_name, node_alias, bx, by, bz, radius )
  
  if create_collision_meshes == false then
  
    return
  end
  
  if shape_type == 0 then        
        
    collisions_transforms[collision_transform_node_alias] = TransformationNode( collision_transform_node_alias, 1 )
    collisions_transforms[collision_transform_node_alias]:LinkTo( scenegraph_name, node_alias )
    local scale = Matrix()
    
    scale:Scale( bx * 2, by * 2, bz * 2 )
    collisions_transforms[collision_transform_node_alias]:UpdateMatrix( 0, scale )
            
    collisions_chunks[collision_chunk_node_alias] = ChunkNode( collision_chunk_node_alias )
    collisions_chunks[collision_chunk_node_alias]:SetMesheName( "cube_meshe" )
    
    for i, pass_name in pairs( collisions_pass ) do
    
      collisions_chunks[collision_chunk_node_alias]:RegisterPassSlot( pass_name )
      collisions_chunks[collision_chunk_node_alias]:SetPassSlotFxName( pass_name, "wireframe_fx" )
      collisions_chunks[collision_chunk_node_alias]:AddPassSlotShaderParam( pass_name, "color", 1, 0, Vector( 1.0, 0.0, 1.0, 1.0 ) )
    
    end
           
    collisions_chunks[collision_chunk_node_alias]:LinkTo( scenegraph_name, collision_transform_node_alias )        
      
  elseif shape_type == 1 then
    
    collisions_transforms[collision_transform_node_alias] = TransformationNode( collision_transform_node_alias, 1 )
    collisions_transforms[collision_transform_node_alias]:LinkTo( scenegraph_name, node_alias )
    local scale = Matrix()
        
    scale:Scale( radius * 2, radius * 2, radius * 2 )
    collisions_transforms[collision_transform_node_alias]:UpdateMatrix( 0, scale )
    
    collisions_chunks[collision_chunk_node_alias] = ChunkNode( collision_chunk_node_alias )
    collisions_chunks[collision_chunk_node_alias]:SetMesheName( "sphere_meshe" )
    
    for i, pass_name in pairs( collisions_pass ) do
    
      collisions_chunks[collision_chunk_node_alias]:RegisterPassSlot( pass_name )
      collisions_chunks[collision_chunk_node_alias]:SetPassSlotFxName( pass_name, "wireframe_fx" )
      collisions_chunks[collision_chunk_node_alias]:AddPassSlotShaderParam( pass_name, "color", 1, 0, Vector( 1.0, 0.0, 1.0, 1.0 ) )
      
    end
    collisions_chunks[collision_chunk_node_alias]:LinkTo( scenegraph_name, collision_transform_node_alias ) 
    
  end

end

print( "Lua version : " .. _VERSION )
ds = DrawSpace()

ds:SetScenegraphNodeEventCallback( function( evt_type, scenegraph_name, node_alias )

  local evt_type_name
  
  if evt_type == 1 then
    evt_type_name = "NODE_APP_READY"
    
    print( "scenegraph event -> " .. node_alias .. " " .. evt_type_name .. " on " .. scenegraph_name )
        
    local inert_body = InertBodyNode( node_alias )
    
    if inert_body:IsValid() == 1 then
      print( "node " .. node_alias .. " is an inert body !!!" )
      
      local shape_type = inert_body:GetShapeType()
      print( "shape type is " .. shape_type )
      
      local collision_chunk_node_alias = node_alias .. "collisionchunk"
      local collision_transform_node_alias = node_alias .. "collisiontransform"
      
      
      create_collision_chunk( shape_type, collision_chunk_node_alias, collision_transform_node_alias, scenegraph_name, node_alias,
                              inert_body:GetShapeDescrBoxDimX(), 
                              inert_body:GetShapeDescrBoxDimY(), 
                              inert_body:GetShapeDescrBoxDimZ(),
                              inert_body:GetShapeDescrSphereRadius() )
      
                              
    else
	  local collider = ColliderNode( node_alias )
	  
	  if( collider:IsValid() == 1 ) then	  
	    print( "node " .. node_alias .. " is a collider !!!" )
	    	          
	    local shape_type = collider:GetShapeType()
	    print( "shape type is " .. shape_type )
      
	    local collision_chunk_node_alias = node_alias .. "collisionchunk"
	    local collision_transform_node_alias = node_alias .. "collisiontransform"
      
      
	    create_collision_chunk( shape_type, collision_chunk_node_alias, collision_transform_node_alias, scenegraph_name, node_alias,
							    collider:GetShapeDescrBoxDimX(), 
							    collider:GetShapeDescrBoxDimY(), 
							    collider:GetShapeDescrBoxDimZ(),
							    collider:GetShapeDescrSphereRadius() )
	        	 
	  end
	  	  	  
    end    
  end
end
)

