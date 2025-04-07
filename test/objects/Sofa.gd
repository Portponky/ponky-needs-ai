extends Node2D

# A relaxing sofa which is a cool place to hang out and socialize

func plan(person: Person, action: Action) -> void:
	if action == %Relax:
		# Sofas are placed in recreational areas
		# Hang around near the sofa for a little bit
		person.do_walk_to(self)
		person.do_loiter(2.0)
		person.do_walk_to(self)
		person.do_loiter(2.0)
		person.do_reward(action.advert)
	
	elif action == %Sit:
		# The dream, let's sit on the sofa and really chill out
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(6.0, 8.0)
		person.do_stop_use(self)
		person.do_reward(action.advert)
