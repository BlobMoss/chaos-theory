#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = position;
	v_TexCoord = texCoord;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color1;
uniform vec4 u_Color2;
uniform vec4 u_Color3;

uniform vec2 u_Pos1;
uniform vec2 u_Pos2;
uniform vec2 u_Pos3;

//lower dt and higher maxSteps increases smoothness, but decreases performance significantly
float dt = 0.01;
int maxSteps = 10000;
vec2 origin = vec2(0.50, 0.50);

vec2 ApplyForces(vec2 pos, vec2 vel, vec2 acc)
{
	//Force asserted by the pendulum string based on hookes law
	vec2 pullAcc = normalize(origin - pos) * 0.05 * distance(pos, origin);

	//Force asserted by the magnets based on the law of gravity
	vec2 magneticAcc1 = normalize(u_Pos1 - pos) * 0.0001 / pow(distance(pos, u_Pos1), 2);
	vec2 magneticAcc2 = normalize(u_Pos2 - pos) * 0.0001 / pow(distance(pos, u_Pos2), 2);
	vec2 magneticAcc3 = normalize(u_Pos3 - pos) * 0.0001 / pow(distance(pos, u_Pos3), 2);

	//Friction proportional to velocity
	vec2 dragAcc = -normalize(vel) * 0.001;

	return pullAcc + magneticAcc1 + magneticAcc2 + magneticAcc3 + dragAcc;
}

vec4 GetColorOfPixel(vec2 pixelCoord)
{
	vec2 pos = vec2(pixelCoord.x, pixelCoord.y);
	vec2 vel = vec2(0.0, 0.0);
	vec2 acc = vec2(0.0, 0.0);

	for (int i = 0; i < maxSteps; i++)
	{
		//An implementation of verlet integration for better physics simulation
		vec2 newPos = pos + vel * dt + acc * (dt * dt * 0.5);
		vec2 newAcc = ApplyForces(pos, vel, acc);
		vec2 newVel = vel + (acc + newAcc) * (dt * 0.5);
		pos = newPos;
		vel = newVel;
		acc = newAcc;

		//Return a color when pendulum has slowed down close to a magnet
		if (length(vel) < 0.25)
		{
			if (distance(pos, u_Pos1) < 0.005) { return u_Color1 * (maxSteps - i) / maxSteps; }
			if (distance(pos, u_Pos2) < 0.005) { return u_Color2 * (maxSteps - i) / maxSteps; }
			if (distance(pos, u_Pos3) < 0.005) { return u_Color3 * (maxSteps - i) / maxSteps; }
		}
	}
	//If no magnet is chosen before i reaches the limit, return black
	return vec4(0.0, 0.0, 0.0, 1.0);
};

void main()
{
	color = GetColorOfPixel(v_TexCoord);
};