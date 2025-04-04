extends StaticBody2D


func plan(person: Person, action: Action) -> void:
	if action == %GetWorker:
		person.workstation = self
		action.active = false
		
		var tags = %DoWork.tags.duplicate()
		tags.append(person.full_name)
		%DoWork.set_tags(tags)
		
		%DoWork.active = true
		person.do_start_use(self)
		person.do_stop_use(self)
		person.do_reward(action.advert)
	elif action == %DoWork:
		person.do_start_use(self)
		person.do_wait(3.0, 5.0)
		person.do_gain("res://objects/Paper.tscn")
		person.do_stop_use(self)
		
