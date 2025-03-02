#ifdef WORKBENCH

[WorkbenchToolAttribute(name: "Object Set Tool", awesomeFontCode: 0xF1B3)]
class OST_ObjectSetTool : WorldEditorTool
{
	private static const int MIN_OBJECT_HEAP_SIZE = 100;
	
	[Attribute("0 0 0", UIWidgets.Coords, "Shifts the position of selected objects", category: "Transform")]
	private vector m_vObjectsPositionOffset;
	
	[Attribute("0 0 0", UIWidgets.Coords, "Shifts the angles of selected objects", category: "Transform")]
	private vector m_vObjectsRotationOffset;
	
	[Attribute("0", UIWidgets.CheckBox, "Should use randomization?", category: "Randomization")]
	private bool m_bEnableRandomization;
	
	[Attribute("1", UIWidgets.Range, "Minimal value for next object scale", category: "Randomization", params: "0 1000")]
	private float m_fMinScale;
	
	[Attribute("1", UIWidgets.Range, "Maximum value for next object scale", category: "Randomization", params: "0 1000")]
	private float m_fMaxScale;
	
	[Attribute("0", UIWidgets.CheckBox, "Calculate next scale of an object relative to the current scale", category: "Randomization")]
	private bool m_bRelativeCurrentScale;
	
	[ButtonAttribute("Apply")]
	private void Apply()
	{
		int selectedEntitiesCount = m_API.GetSelectedEntitiesCount();
		WBProgressDialog progressDialog;
		
		if (selectedEntitiesCount >= MIN_OBJECT_HEAP_SIZE)
		{
			WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
			
			string message = string.Format("Processing %1 entities...", selectedEntitiesCount);
			progressDialog = new WBProgressDialog(message, worldEditor);
			
			progressDialog.SetProgress(0);
		}
		
		m_API.BeginEntityAction();
		
		for (int n = 0; n < selectedEntitiesCount; n++)
		{
			IEntitySource entitySource = m_API.GetSelectedEntity(n);
			IEntity entity = m_API.SourceToEntity(entitySource);
			
			if (!entity) continue;
			
			vector position = entity.GetOrigin();
			position += m_vObjectsPositionOffset;
			
			vector angles = entity.GetAngles();
			string angleKeys[3] = {"angleX", "angleY", "angleZ"};
			
			float currentEntityScale = 1;
			
			if (m_bEnableRandomization && m_bRelativeCurrentScale)
				currentEntityScale = entity.GetScale();
			
			for (int i = 0; i < 3; i++)
			{
				float angle = angles[i] + m_vObjectsRotationOffset[i];
				m_API.SetVariableValue(entitySource, null, angleKeys[i], angle.ToString(false));
			}
			
			m_API.SetVariableValue(entitySource, null, "coords", position.ToString(false));
			
			if (m_bEnableRandomization)
			{
				float minScale = Math.Clamp(Math.Min(m_fMinScale, m_fMaxScale), 0, 1000);
				float maxScale = Math.Clamp(Math.Max(m_fMinScale, m_fMaxScale), 0, 1000);
					
				float randomScaleValue = Math.RandomFloat(minScale, maxScale);
				
				if (m_bRelativeCurrentScale)
				{
					float nextScaleValue = Math.AbsFloat(currentEntityScale * randomScaleValue);
					m_API.SetVariableValue(entitySource, null, "scale", nextScaleValue.ToString(false));
				}
				else
				{
					m_API.SetVariableValue(entitySource, null, "scale", randomScaleValue.ToString(false));
				}
			}
			
			if (progressDialog)
				progressDialog.SetProgress((n + 1) / selectedEntitiesCount);
		}
		
		m_API.EndEntityAction();
	}
}

#endif