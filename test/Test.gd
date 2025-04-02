extends Node2D

@export var mouse_selection_shape: Shape2D
@onready var shape_query = PhysicsShapeQueryParameters2D.new()

const PERSON_LIMIT = 20

func _ready() -> void:
	var person_count = 0
	for node in $Things.get_children():
		if node is not Person:
			continue
		
		person_count += 1
		if person_count > PERSON_LIMIT:
			node.queue_free()
	
	shape_query.collide_with_areas = true
	shape_query.collide_with_bodies = true
	shape_query.collision_mask = 2
	shape_query.shape = mouse_selection_shape


func _process(_delta: float) -> void:
	shape_query.transform = Transform2D.IDENTITY.translated(get_global_mouse_position())
	var results = get_world_2d().direct_space_state.intersect_shape(shape_query, 1)
	if results.is_empty():
		%PopupLayer.visible = false
		return
	
	var person = results[0].collider as Person
	if !person:
		%PopupLayer.visible = false
		return
	
	%PopupLayer.visible = true
	%Popup.position = person.position
	%Popup.position.x -= %Popup.size.x / 2
	%Name.text = person.full_name
	
	var values := person.get_need_values()
	%ThirstValue.value = values.thirst
	%HungerValue.value = values.hunger
	%WorkValue.value = values.work
	%SocialValue.value = values.social
	%CleanValue.value = values.clean
