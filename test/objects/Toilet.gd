extends Node2D

@export var bathroom_broadcast := false

func _ready() -> void:
	%Beacon.active = bathroom_broadcast

func plan(person: Person, action: Action) -> void:
	if action == %Poop:
		person.do_start_use(self)
		person.do_wait(6.0, 8.0)
		person.do_stop_use(self)
		person.do_loiter(0.4)
		person.do_reward(action.advert)
		
		action.active = false
		%Timer.start()
	elif action == %Beacon:
		person.do_walk_to(global_position + Vector2(0, 20))
		person.do_loiter(0.2)


func _on_timer_timeout() -> void:
	%Poop.active = true
