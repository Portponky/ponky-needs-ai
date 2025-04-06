class_name Person extends CharacterBody2D

const WALK_SPEED := 50.0
const LOITER_SPEED := 15.0
const USE_ATTEMPTS := 10

# Tasks that a person can do
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
	QUEUE_FREE, # disappear
}

## Person's name, which is unique
@export var full_name : String

var desk : Node2D # Their desk
var held : Node2D # The object they are carrying

# The smile indicator when talking
@onready var chat_indicator = $ChatIndicator

# What is the person doing
var _current_task : int

# General waiting
var _wait_left : float # how long to wait before next task

# Walking
var _walk_direction : Vector2
var _walk_speed := 0.0

# Trying to use something
var _use_attempts := USE_ATTEMPTS

var _task_queue := []
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
	%Agent.tags.append(full_name)
	
	# Prevent the chat action from triggering with the agent
	# e.g. can't chat to yourself
	%Chat.tags = ["-" + full_name]
	
	# Wait for a little bit before doing something
	do_wait(0.5, 1.0)
	start_next_task()


# Populating the task queue
func do_wait(min_time: float, max_time: float) -> void:
	var time = randf_range(min_time, max_time)
	_task_queue.append([Task.WAIT, time])


func do_walk_to(object: Node2D) -> void:
	# Walk to about 12 pixels away from a position
	_task_queue.append([Task.WALK_TO, object.global_position, 12.0])


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
	# When using an object, walk directly in front of it
	_task_queue.append([Task.WALK_TO, object.global_position + Vector2(0.0, 8.0), 2.0])


func do_stop_use(object: Node2D) -> void:
	_task_queue.append([Task.USE, object, false])


func do_loiter(time: float) -> void:
	_task_queue.append([Task.LOITER, time])


func do_reward(grant: Dictionary[String, float]) -> void:
	_task_queue.append([Task.REWARD, grant])


func do_queue_free() -> void:
	_task_queue.append([Task.QUEUE_FREE])


func do_stash_tasks() -> void:
	# If the person is doing something but needs to schedule some tasks beforehand
	# stash the current tasks into another array to free up the task queue
	_task_queue.append_array(_stashed_task_queue)
	_stashed_task_queue = _task_queue
	_task_queue = []


func start_next_task() -> void:
	# Make sure any stashed commands are appended to the task queue
	_task_queue.append_array(_stashed_task_queue)
	_stashed_task_queue.clear()
	
	%Chat.active = false
	
	# Nothing to do? Think of something
	if _task_queue.is_empty():
		if _current_task != Task.THINKING:
			_current_task = Task.THINKING
			var tags := []
			if not desk:
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
				# this object is being used by someone else
				_use_attempts -= 1
				if _use_attempts == 0:
					fail_task()
					return
				
				do_stash_tasks()
				do_loiter(0.8)
				do_start_use(obj)
			else:
				if use:
					obj.add_to_group("being-used")
				else:
					obj.remove_from_group("being-used")
				# reset attempts for next time
				_use_attempts = USE_ATTEMPTS
		
		Task.LOITER:
			# Let's loiter for a bit, and possibly chat
			%Chat.active = true
			_walk_direction = Vector2.RIGHT.rotated(randf() * TAU)
			_wait_left = task[1]
		
		Task.QUEUE_FREE:
			if desk:
				desk.unassign()
			queue_free()


func fail_task() -> void:
	# Can't complete the current task
	# Either trying to walk to a destination that's unreachable
	# or can't use an item because it's in use
	_task_queue.clear()
	_stashed_task_queue.clear()
	
	_use_attempts = USE_ATTEMPTS
	
	start_next_task()



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
	if abs(_walk_direction.x) > 0.15:
		%Sprite.flip_h = _walk_direction.x < 0.0
	
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
		if not %NavAgent.is_target_reachable():
			fail_task()
	elif _current_task == Task.LOITER:
		velocity = LOITER_SPEED * _walk_direction
		if move_and_slide():
			_walk_direction = Vector2.RIGHT.rotated(randf() * TAU)
	
	# if this is a task that times out, check the time out
	if _current_task not in [Task.THINKING, Task.WALK_TO]:
		_wait_left -= delta
		if _wait_left < 0.0:
			start_next_task()


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
		person.do_walk_to(self)
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
