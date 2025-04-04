class_name Person extends CharacterBody2D

@export var full_name : String
var workstation : Node2D
var held : Node2D
@onready var chat_indicator = $ChatIndicator

enum Task {
	THINKING, # waiting for a response from utility server
	WAIT, # for a length of time
	WALK_TO, # a give destination point
	GAIN, # an item appears in the person's hand
	DESTROY, # item in hand ceases to exist
	GRAB, # an item which is nearby to hand
	DROP, # place item in hand on the floor
	USE, # control exclusive access to something
	LOITER, # move around at random
	REWARD, # grant a specific advert
}

var _current_task := Task.WAIT

var _wait_left : float

const WALK_SPEED := 50.0
const LOITER_SPEED := 15.0
var _walk_direction : Vector2
var _walk_speed := 0.0

@onready var _task_queue := []
var _stashed_task_queue := []

func _ready() -> void:
	# Let's add some noise to the initial values of our agents
	var noise : Dictionary[String, float] = {
		"hunger": randf_range(-0.2, 0.1),
		"thirst": randf_range(-0.5, 0.1),
		"work": randf_range(-0.2, 0.2),
		"social": randf_range(-0.1, 0.1),
		"clean": randf_range(-0.3, 0.2),
	}
	%Agent.grant(noise)
	
	# Agents are tagged with the person's name for specific actions
	# This assumes no workers have the same name, which is fine for this example
	%Agent.tags.append(full_name)
	
	%Chat.tags = ["-" + full_name]
	
	# Wait for a little bit before doing something
	do_wait(1.0, 2.0)
	start_next_task()


# Populating the task queue
func do_wait(min_time: float, max_time: float) -> void:
	var time = randf_range(min_time, max_time)
	_task_queue.append([Task.WAIT, time])


func do_walk_to(pos: Vector2) -> void:
	_task_queue.append([Task.WALK_TO, pos, 12.0])


func do_reward(grant: Dictionary[String, float]) -> void:
	_task_queue.append([Task.REWARD, grant])


func do_gain(scene: String) -> void:
	_task_queue.append([Task.GAIN, scene])


func do_destroy() -> void:
	_task_queue.append([Task.DESTROY])


func do_grab(object: Node2D) -> void:
	_task_queue.append([Task.GRAB, object])


func do_drop() -> void:
	_task_queue.append([Task.DROP])


func do_start_use(object: Node2D) -> void:
	_task_queue.append([Task.USE, object, true])
	_task_queue.append([Task.WALK_TO, object.global_position + Vector2(0.0, 8.0), 2.0])


func do_stop_use(object: Node2D) -> void:
	_task_queue.append([Task.USE, object, false])


func do_loiter(time: float) -> void:
	_task_queue.append([Task.LOITER, time])


func do_stash_tasks() -> void:
	_task_queue.append_array(_stashed_task_queue)
	_stashed_task_queue = _task_queue
	_task_queue = []


func start_next_task() -> void:
	_task_queue.append_array(_stashed_task_queue)
	_stashed_task_queue.clear()
	
	if _task_queue.is_empty():
		if _current_task != Task.THINKING:
			_current_task = Task.THINKING
			var tags := []
			if not workstation:
				tags.append("unassigned")
			if not held:
				tags.append("empty-handed")
			else:
				# default to scene file name in lowercase
				var type = held.scene_file_path.get_file().left(-5).to_lower()
				# but if the scene has an object_name variable, use that instead
				if "object_name" in held:
					type = held.object_name
				
				tags.append("holding-" + type)
				
				if held.is_in_group("drink"):
					tags.append("holding-drink")
				elif held.is_in_group("food"):
					tags.append("holding-food")
			
			%Agent.choose_action(150.0, 250.0, tags)
		return
	
	%Chat.active = false
	
	var task = _task_queue.pop_front()
	_current_task = task[0]
	
	_wait_left = 0.1
	match _current_task:
		Task.WAIT:
			_wait_left = task[1]
		
		Task.WALK_TO:
			_walk_direction = Vector2.ZERO
			%NavAgent.target_desired_distance = task[2]
			%NavAgent.target_position = task[1]
			_wait_left = -1
		
		Task.REWARD:
			var reward : Dictionary[String, float] = task[1]
			%Agent.grant(reward)
		
		Task.GAIN:
			if held:
				drop_object()
			
			var object = load(task[1]).instantiate()
			grab_object(object)
		
		Task.DESTROY:
			if held:
				held.queue_free()
				held = null
		
		Task.GRAB:
			if held:
				drop_object()
			if is_instance_valid(task[1]):
				grab_object(task[1])
		
		Task.DROP:
			if held:
				drop_object()
		
		Task.USE:
			var obj : Node2D = task[1]
			var use : bool = task[2]
			var being_used = obj.is_in_group("being-used")
			
			if being_used and use:
				do_stash_tasks()
				do_loiter(0.8)
				do_start_use(obj)
			else:
				if use:
					obj.add_to_group("being-used")
				else:
					obj.remove_from_group("being-used")
		
		Task.LOITER:
			%Chat.active = true
			_walk_direction = Vector2.RIGHT.rotated(randf() * TAU)
			_wait_left = task[1]


func _on_agent_action_chosen(action: Action) -> void:
	# probe action for what to do
	var node = action.owner
	if node.has_method("plan"):
		node.plan(self, action)
	else:
		print("Action ", action, " has no plan method")
		do_wait(5.0, 10.0)
	
	start_next_task()


func _on_agent_no_action_chosen() -> void:
	# Not much work doing. Loiter for a bit.
	do_loiter(5.0)
	start_next_task()


func pick_random_point_goal() -> void:
	# This is a bit silly, for testing!
	var maps := NavigationServer2D.get_maps()
	%NavAgent.target_position = NavigationServer2D.map_get_random_point(maps.front(), 1, true)


func drop_object() -> Node2D:
	if !held:
		return
	
	remove_child(held)
	held.position = position
	add_sibling(held)
	
	var dropped = held
	held = null
	return dropped


func grab_object(object: Node2D) -> void:
	if held:
		return
	
	if object.is_inside_tree():
		object.get_parent().remove_child(object)
	object.position = %Hand.position
	add_child(object)
	held = object


func _process(delta: float) -> void:
	if _current_task == Task.WALK_TO:
		var offset : Vector2 = %NavAgent.get_next_path_position() - position
		var ideal := offset.normalized()
		
		if _walk_direction == Vector2.ZERO:
			_walk_direction = ideal
		
		_walk_speed = lerp(_walk_speed, WALK_SPEED, 0.5)
		velocity = _walk_speed * _walk_direction
		if move_and_slide():
			ideal = get_last_slide_collision().get_normal()
			_walk_speed = max(0.0, _walk_speed - 10.0)
	
		var angle := _walk_direction.angle_to(ideal)
		angle = clampf(angle, -0.2, 0.2)
		_walk_direction = _walk_direction.rotated(angle)
	
		if %NavAgent.is_target_reached() or %NavAgent.is_navigation_finished():
			start_next_task()
	elif _current_task == Task.LOITER:
		velocity = LOITER_SPEED * _walk_direction
		if move_and_slide():
			_walk_direction = Vector2.RIGHT.rotated(randf() * TAU)
	
	if _wait_left >= 0.0:
		_wait_left -= delta
		if _wait_left < 0.0:
			start_next_task()
	
	if abs(velocity.x) > 5.0:
		%Sprite.flip_h = velocity.x < 0.0


func get_need_values() -> Dictionary:
	return {
		"hunger": %Agent.get_need_value("hunger"),
		"thirst": %Agent.get_need_value("thirst"),
		"work": %Agent.get_need_value("work"),
		"social": %Agent.get_need_value("social"),
		"clean": %Agent.get_need_value("clean"),
	}


func plan(person: Person, action: Action) -> void:
	if action == %Chat:
		# someone's chatting with me!!
		person.do_walk_to(global_position)
		person.do_wait(3.0, 3.0)
		person.do_reward(action.advert)
		
		# fix the front pushing here, and in general
		do_stash_tasks()
		do_wait(4.0, 4.0)
		do_reward(action.advert)
		start_next_task()
		
		chat_indicator.visible = true
		person.chat_indicator.visible = true
		await get_tree().create_timer(3.0).timeout
		chat_indicator.visible = false
		person.chat_indicator.visible = false
