extends Node2D

const PERSON = preload("res://Person.tscn")

@export var mouse_selection_shape: Shape2D
@onready var shape_query = PhysicsShapeQueryParameters2D.new()

const PERSON_LIMIT = 20

var forenames : Array[String]
var surnames : Array[String]
var used_names := {}

func _ready() -> void:
	forenames = load_text_as_string_array("res://assets/forename.txt")
	surnames = load_text_as_string_array("res://assets/surname.txt")

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


func load_text_as_string_array(filename: String) -> Array[String]:
	var file = FileAccess.open(filename, FileAccess.READ)
	var result : Array[String] = []
	
	while not file.eof_reached():
		result.append(file.get_line())
	
	# last entry is blank
	if result.back().is_empty():
		result.pop_back()
	
	return result


func create_person() -> Person:
	var generated_name := ""
	while generated_name.is_empty() or used_names.has(generated_name):
		generated_name = "%s %s" % [forenames.pick_random(), surnames.pick_random()]
	
	var person = PERSON.instantiate()
	person.full_name = generated_name
	person.global_position = %Spawn.global_position
	%Things.add_child(person)
	
	used_names[generated_name] = person
	person.tree_exited.connect(func():
		used_names.erase(generated_name)
	)
	return person


func _process(_delta: float) -> void:
	%Info.text = "%d people\n%.1f fps" % [used_names.size(), Engine.get_frames_per_second()]
	
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


func _on_add_person_pressed() -> void:
	var person = create_person()
	print("%s arrived" % person.full_name)


func _on_remove_person_pressed() -> void:
	if used_names.is_empty():
		return
	
	var target = used_names.keys().pick_random()
	var person : Person = used_names[target]
	
	# Let's remove this person
	person.do_walk_to(%Despawn.global_position)
	person.do_queue_free()
