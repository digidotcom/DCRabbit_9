
function Setup()
{
	for(i=0;i<max_adc_cfg_boxes;i++)
   	AdcConfig(i);

   DioSetup();
   DinSetup();
   HcoSetup();
}

function DioSetup()
{
	for(i=15;i>=0;i--)
   {
      var htmlc = "<input type='checkbox' id='dioc'"+i+" name='dioc'"+i+" onclick='DioSet(0,"+i+")' ";
      var htmlv = "<input type='checkbox' id='diov'"+i+" name='diov'"+i+" onclick='DioSet(1,"+i+")' ";
      var cfg	 = document.getElementById("dioc"+i);
      var val   = document.getElementById("diov"+i);

      if(dioc&(Math.pow(2,i)))
      	cfg.innerHTML = htmlc+"value='1' checked='checked' />";
      else {
         cfg.innerHTML = htmlc+"value='0' />";
         htmlv = htmlv+" disabled='disabled' "; }

      if(diov&(Math.pow(2,i)))
      	val.innerHTML = htmlv+"value='1' checked='checked' />";
      else
      	val.innerHTML = htmlv+"value='0' />";
   }
}


function DioSet(box,io)
{
   switch(box)
   {
   	case 0:
         dioc = dioc^(1<<io);
   		document.getElementById(d_type+d_num+".diox.cfg").value = dioc;
      break;
      case 1:
         diov = diov^(1<<io);
         document.getElementById(d_type+d_num+".diox.val").value = diov;
      break;
   }
}

function DinSetup()
{
	for(i=15;i>=0;i--)
   {
      var htmlv = "<input type='checkbox' id='dinx'"+i+" name='dinx'"+i+
                  " onclick='DinSet("+i+")' disabled='disabled' ";
      var val   = document.getElementById("dinx"+i);

      if(dinx&(Math.pow(2,i)))
      	val.innerHTML = htmlv+"value='1' checked='checked' />";
      else
         val.innerHTML = htmlv+"value='0' />";
   }
}

function DinSet(io)
{
   dinx = dinx^(1<<io);
   document.getElementById(d_type+d_num+".dinx").value = dinx;
}

function HcoSetup()
{
	for(i=3;i>=0;i--)
   {
      var htmlc = "<input type='checkbox' id='hcoc'"+i+" name='hcoc'"+i+" onclick='HcoSet(0,"+i+")' ";
   	var htmlv = "<input type='checkbox' id='hcov'"+i+" name='hcov'"+i+" onclick='HcoSet(1,"+i+")' ";
   	var cfg   = document.getElementById("hcoc"+i);
   	var val   = document.getElementById("hcov"+i);

      if(hcoc&(Math.pow(2,i)))
      	cfg.innerHTML = htmlc+"value='1' checked='checked' />";
      else {
         cfg.innerHTML = htmlc+"value='0' />";
         //htmlv = htmlv+" disabled='disabled' ";
      }

      if(hcov&(Math.pow(2,i)))
      	val.innerHTML = htmlv+"value='1' checked='checked' />";
      else
      	val.innerHTML = htmlv+"value='0' />";
   }
}

function HcoSet(box,io)
{
   switch(box)
   {
   	case 0:
         hcoc = hcoc^(1<<io);
   		document.getElementById(d_type+d_num+".hcox.cfg").value = hcoc;
      break;
      case 1:
         hcov = hcov^(1<<io);
         document.getElementById(d_type+d_num+".hcox.val").value = hcov;
      break;
   }
}

function AdcConfig(io)
{
	var cfg = document.getElementById(d_type+d_num+".adc_cfg "+io+" ");
   var unt = document.getElementById(d_type+d_num+".adc "+io+" .unt");
   var val_pair = document.getElementById(d_type+d_num+".adc "+(io*2+1)+" .val");
   var unt_pair = document.getElementById(d_type+d_num+".adc "+(io*2+1)+" .unt");

   switch(cfg.selectedIndex)
   {
   	 case 3:										// single mAmp mode
			val_pair.style.color = "black"	// reset the font color
			unt_pair.disabled = false;			// re-enable the units menu
       break;
       case 2: 									// differential mode so disable pair
		 	val_pair.style.color = "gray";	// set font to gray
			unt_pair.disabled = true;			// disable the units menu
       break;
       default:									// single Volt mode
         val_pair.style.color = "black"	// reset the font color
		 	unt_pair.disabled = false;			// re-enable the units menu
       break;
   }
}


function MyUpdate()
{
	var upd = document.getElementById("dev.update");

   if(upd.value == 1)
   	upd.value = 0;
   else
   	upd.value = 1;
}