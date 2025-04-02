extends Node2D

func plan(person: Person, action: Action) -> void:
	if action == %Poop:
		person.do_start_use(self)
		person.do_wait(6.0, 8.0)
		person.do_stop_use(self)
		person.do_loiter(0.4)
		person.do_reward(action.advert)
		
		action.active = false
		%Timer.start()


func _on_timer_timeout() -> void:
	%Poop.active = true
