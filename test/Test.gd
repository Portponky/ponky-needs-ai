extends Node2D

const PERSON = preload("res://Person.tscn")

# The people have a very small bounding box, so use a shape query when finding them
@export var mouse_selection_shape: Shape2D
@onready var shape_query = PhysicsShapeQueryParameters2D.new()

# List of names for name generation
var forenames : Array[String]
var surnames : Array[String]
var used_names := {}

func _ready() -> void:
	# Load names
	forenames = load_text_as_string_array("res://assets/forename.txt")
	surnames = load_text_as_string_array("res://assets/surname.txt")
	
	# Initialize shape query for mouse-over
	shape_query.collide_with_areas = true
	shape_query.collide_with_bodies = true
	shape_query.collision_mask = 2
	shape_query.shape = mouse_selection_shape


func load_text_as_string_array(filename: String) -> Array[String]:
	var file = FileAccess.open(filename, FileAccess.READ)
	var result : Array[String] = []
	
	while not file.eof_reached():
		result.append(file.get_line())
	
	# Last entry is blank
	if result.back().is_empty():
		result.pop_back()
	
	return result


func create_person() -> Person:
	# Pick a name
	var generated_name := ""
	while generated_name.is_empty() or used_names.has(generated_name):
		generated_name = "%s %s" % [forenames.pick_random(), surnames.pick_random()]
	
	# Create the person
	var person = PERSON.instantiate()
	person.full_name = generated_name
	person.global_position = %Spawn.global_position
	%Things.add_child(person)
	
	# Set up the name so it doesn't get reused
	used_names[generated_name] = person
	person.tree_exited.connect(func():
		used_names.erase(generated_name)
	)
	return person


func _process(_delta: float) -> void:
	# Update corner text
	%Info.text = "%d people\n%.1f fps" % [used_names.size(), Engine.get_frames_per_second()]
	
	# Query the mouse area
	shape_query.transform = Transform2D.IDENTITY.translated(get_global_mouse_position())
	var results = get_world_2d().direct_space_state.intersect_shape(shape_query, 1)
	if results.is_empty():
		%PopupLayer.visible = false
		return
	
	var person = results[0].collider as Person
	if !person:
		%PopupLayer.visible = false
		return
	
	# Found a person, fill in the details
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
	
	# Select a person at random
	var target = used_names.keys().pick_random()
	var person : Person = used_names[target]
	
	# Let's compel this person to leave
	# This might fail if they fail their current task (e.g. waiting for something)
	# So it isn't guaranteed they will leave
	person.do_walk_to(%Despawn)
	person.do_queue_free()


func _on_pause_toggled(pause: bool) -> void:
	get_tree().paused = pause
