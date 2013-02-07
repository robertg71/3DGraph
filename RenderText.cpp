/*
 * Text.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: CodeArt
 */

#include <mavsprintf.h>
#include "RenderText.h"
#include "graph.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


namespace Graph{
// A SIMPLYFIED TEXT RENDERING CLASS
//-----------------------------------------------------

RenderText::RenderText()
:	m_font(0),
	m_width(0),
	m_height(0),
/*	m_pContext(0),
	m_pDevice(0),
	m_pVertexBuffer(0),
	m_pIndexBuffer(0),
	m_pSampleState(0),
	m_alphaDisableBlendingState(0),
	m_alphaEnableBlendingState(0), */
	m_world(1),
	m_view(1),
	m_proj(1)
{
}

RenderText::~RenderText()
{
	Release();
}

void RenderText::Release()
{
/*
	SAFE_RELEASE(m_alphaEnableBlendingState);
	SAFE_RELEASE(m_alphaDisableBlendingState);
	SAFE_RELEASE(m_pSampleState);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
*/
}
// Suppose we could use text box support with the width / height
bool RenderText::Init(/*LPDIRECT3DDEVICE pDevice, LPDEVICECONTEXT pContext,*/ float width, float height, BMFont *font)
{
	m_width		= width;
	m_height	= height;
//	m_pDevice	= pDevice;
//	m_pContext	= pContext;
	m_font		= font;

	m_textShader.init();
	// This could be commonly used.
	// TODO create a common blend state class that holds all blendstates we need.
/*
	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable			= TRUE;
    blendStateDescription.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	HRESULT result = pDevice->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if(FAILED(result))
		return false;

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = pDevice->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if(FAILED(result))
	{
		SAFE_RELEASE(m_alphaEnableBlendingState);	// release first blend state.
		return false;
	}

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;

	memset(&samplerDesc,0,sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias		= 0.0f;
	samplerDesc.MaxAnisotropy	= 1;
	samplerDesc.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;	// D3D11_DEPTH_STENCIL_DESC should do the job unless we want to deal with deffered system
	samplerDesc.BorderColor[0]	= 0;
	samplerDesc.BorderColor[1]	= 0;
	samplerDesc.BorderColor[2]	= 0;
	samplerDesc.BorderColor[3]	= 0;
	samplerDesc.MinLOD			= 0;
	samplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = pDevice->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if(FAILED(result))
		return false;

	if (m_font)
		createDefaultBuffers(256);

	// initiate ShaderStore vtx & pix shader for FONT.
	std::string name = "font";
	Shaders sh = SHADER_FONT;
	g_globals.Dx().CreateShader(pDevice,&m_pShader,name,sh); */
	return true;
}

void RenderText::releaseDefaultBuffer()
{
//	SAFE_RELEASE(m_pVertexBuffer);
//	SAFE_RELEASE(m_pIndexBuffer);
}

bool RenderText::createDefaultBuffers(int strSz)
{
/*	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	// make sure that any previous vertex or index buffer are cleared
	releaseDefaultBuffer();

	const int nChars = strSz;
	const int sz = nChars*6;	// 4 vertices (quad) / character (sprite)

	int numElements,strideVtx,strideInstanceDmy;
	GetVertexElement(SHADER_FONT, numElements, strideVtx, strideInstanceDmy);

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage					= D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth				= strideVtx * sz;
	vertexBufferDesc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags				= 0;
	vertexBufferDesc.StructureByteStride	= 0;

	// Create the vertex buffer.
	HRESULT result = m_pDevice->CreateBuffer(&vertexBufferDesc, NULL, &m_pVertexBuffer);
	if(FAILED(result))
		return false;

	const int isz = 6*nChars;

	// Create the index array.
	unsigned short *indices = new unsigned short[isz];
	if(!indices)
		return false;

	// Initialize the index array.
	for(int i=0; i<isz; i++)
		indices[i] = i;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth			= sizeof(short) * isz;
	indexBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags		= 0;
	indexBufferDesc.MiscFlags			= 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem					= indices;
	indexData.SysMemPitch				= 0;
	indexData.SysMemSlicePitch			= 0;

	// Create the index buffer.
	result = m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	delete [] indices;

	return (FAILED(result))? false: true;*/
	return true;
}

void RenderText::SetFont(BMFont *font)
{
	m_font = font;
}

// should have a separate draw text a direct and a cashed.
float RenderText::DrawText(const char*str,float x,float y, glm::vec4 &rgba, Scene &scene)
{
	checkGLError("RenderText::DrawText   Should be ok!");

	float width = 0;

	float drawX = 0.0f;//(float)(((m_width / 2.0f) * -1.0f) + x);
	float drawY = 0.0f;//(float)((m_height / 2.0f) - y);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.

	// Create the vertex array.
	int num			= strlen(str);											// get number chars

	lprintfln("RenderText::DrawText size=%d string=\"%s\"\n",num,str);
	glm::vec3 *vertices	= new glm::vec3[6*num];									// quad  * num (quad is 2 tri = 6 vertices / char)
	if(!vertices)
		return false;
	glm::vec2 *texCoord = new glm::vec2[6*num];
	if(!texCoord)
		return false;

//	glDisable(GL_CULL_FACE);

	width = m_font->BuildVertexArray(vertices,texCoord, str, drawX, drawY, 0.10f, 0.10f);			// get vertex array from string,
/*
	int cnt = 0;
	for(int i=0; i<6*num;)
	{
		lprintfln("RenderText: Polygon=%d",cnt);
		lprintfln("%d.RenderText:v(%f,%f,%f) t(%f,%f)",i,vertices[i].x,vertices[i].y,vertices[i].z,texCoord[i].x,texCoord[i].y);
		i++;
		lprintfln("%d.RenderText:v(%f,%f,%f) t(%f,%f)",i,vertices[i].x,vertices[i].y,vertices[i].z,texCoord[i].x,texCoord[i].y);
		i++;
		lprintfln("%d.RenderText:v(%f,%f,%f) t(%f,%f)",i,vertices[i].x,vertices[i].y,vertices[i].z,texCoord[i].x,texCoord[i].y);
		i++;
		cnt++;
	}
*/
	TextShader &shader= m_textShader;

	lprintfln("shader.mShader = %d",shader.mShader);
	// Use the program object
	glUseProgram(shader.mShader);
	checkGLError("RenderText::DrawText   glUseProgram (Text)");

	// 1. create buffers tri array for simplicity? so that every tri can be handled separately or with a new index buffer could be expensive though???
	// Load the vertex data

	glVertexAttribPointer(shader.mAttribVtxLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, &vertices[0].x);
	checkGLError("RenderText::DrawText   glVertexAttribPointer (V)");

	glEnableVertexAttribArray(shader.mAttribVtxLoc);
	checkGLError("RenderText::DrawText   glEnableVertexAttribArray (V)");

	glVertexAttribPointer(shader.mAttribTCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, &texCoord[0].x);
	checkGLError("RenderText::DrawText   glVertexAttribPointer (TC)");

	glEnableVertexAttribArray(shader.mAttribTCoordLoc);
	checkGLError("RenderText::DrawText   glEnableVertexAttribArray (TC)");


//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D,  m_font->GetTexture(0));
/*    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, ...);
	*/

	glActiveTexture(GL_TEXTURE0 + 0);
	checkGLError("RenderText::DrawText   glActiveTexture");
//	glEnable(GL_TEXTURE_2D);
//	checkGLError("RenderText::DrawText   glEnable (Text)");
	glBindTexture(GL_TEXTURE_2D, m_font->GetTexture(0));
	checkGLError("glBindTexture");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGLError("glTexParameteri");
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );



    //	checkGLError("Bind Texture");
//	glBindSampler(0, linearFiltering);
	// Update variables to the shader, that is only updated commonly for all bars once per frame such as ParojactionMatrix, ViewMatrix, should be World Matrix aswell
	// projectionMatrix and viewMatrix tick time, resolution constants for pixel shader that are identical trough out the obj calls. hence update only once.
	glUniform1i(shader.mTexture, 0 ); // Texture Unit 0
	checkGLError("RenderText::DrawText   glUniform1i");

	// Once per frame
	glUniform1f(shader.mTimeLoc, scene.getTick());
	checkGLError("RenderText::DrawText   glUniform1f");
	glUniform2f(shader.mResolutionLoc, 1.0f/(float)scene.getWidth(), 1.0f/(float)scene.getHeight());
	checkGLError("RenderText::DrawText   glUniform2f");
	glUniformMatrix4fv(shader.mMatrixP, 1, GL_FALSE, &scene.getProjectionMat()[0][0]);
	checkGLError("RenderText::DrawText   glUniformMatrix4fv");
	glUniformMatrix4fv(shader.mMatrixV, 1, GL_FALSE, &scene.getViewMat()[0][0]);
	checkGLError("RenderText::DrawText   glUniformMatrix4fv");

	glm::vec3 sv(1.0f, 1.0f, 1.0f);
	glUniform3fv(shader.mScaleV,1, (float *)&sv.x);				// mScale location => variable "ScaleV" in vertex shader
	checkGLError("RenderText::DrawText   glUniformMatrix3fv");

	glm::vec4 color(1.0f,1.0f,1.0f,1.0f);
	glUniform4fv(shader.mColor,1, (float *)&color.x);
	checkGLError("RenderText::DrawText   glUniformMatrix4fv");

	// Allways
	glm::mat4 m 	= glm::translate(scene.getWorldMat(),-5.0f,0.0f,1.0f);	// does a mat mul
	glUniformMatrix4fv(shader.mMatrixM, 1, GL_FALSE, &m[0][0]);	// to the mMatrix Location => variable "World" in vertex shader
	checkGLError("RenderText::DrawText   glUniformMatrix4fv");

	glDrawArrays(GL_TRIANGLES, 0, 6*num);
	checkGLError("RenderText::DrawText   glDrawArrays");


	// Clean-up
	glDisableVertexAttribArray(shader.mAttribVtxLoc);
	checkGLError("RenderText::DrawText   glDisableVertexAttribArray (Vtx)");
	glDisableVertexAttribArray(shader.mAttribTCoordLoc);
	checkGLError("RenderText::DrawText   glDisableVertexAttribArray (TC)");

//	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	checkGLError("RenderText::DrawText   glUseProgram(0)");

	delete [] vertices;
	delete [] texCoord;

	// Generate a buffer for the vertices

	//	glGenBuffers(1, &mShader.mVertexbuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, mShader.mVertexbuffer);
//	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);


	/*
void draw() {
		GLfloat vVertices[] = { -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
				-1.0f, 0.0f, -1.0f, -1.0f, 0.0f };

		// Set the viewport
		glViewport(0, 0, mWidth, mHeight);

		// Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// Use the program object
		glUseProgram(mShader);
		checkGLError("glUseProgram");

		glUniform1f(mTimeLoc, (maGetMilliSecondCount() - mStartTime) * 0.001f);
		checkGLError("glUniform1f");

		glUniform2f(mResolutionLoc, 1.0f/(float)mWidth, 1.0f/(float)mHeight);
		checkGLError("glUniform2f");

		// Load the vertex data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
		checkGLError("glVertexAttribPointer");

		glEnableVertexAttribArray(0);
		checkGLError("glEnableVertexAttribArray");

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		checkGLError("glDrawArrays");

	}
};	 *
	 */

	// 2. try to reuse the buffer if possible...
/*
	// Lock the vertex buffer so it can be written to.
	HRESULT result = m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);	// get map
	if(FAILED(result))
	{
		delete [] vertices;
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (BYTE*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sz*6*num));

	// Unlock the vertex buffer.
	m_pContext->Unmap(m_pVertexBuffer, 0);

	delete [] vertices;
	vertices = 0;

	m_matrixParam[0] = &m_proj;
	m_matrixParam[1] = &m_view;
	m_matrixParam[2] = &m_world;

	m_pShader->renderUpdates(m_pDevice,(void*)&FontParam(&rgba,m_matrixParam,1),1);	// Shader updates, preparing constant tables etc.

	{
		VertexShaderStore*	stv		= m_pShader->getVertexShaderStore();
		PixelShaderStore*	stp		= m_pShader->getPixelShaderStore();
		LPXCONSTANTTABLE	cbufv	= stv->ConstantTable();
		LPXCONSTANTTABLE	cbufp	= stp->ConstantTable();
		m_pContext->VSSetShader( stv->VertexShader(), NULL, 0 );
		if (cbufv)
			m_pContext->VSSetConstantBuffers( 0, 1, &cbufv );			// note for future ref, they might not need to be updated all the time.. but for time being better safe then sorry

		m_pContext->PSSetShader( stp->PixelShader(), NULL, 0 );
		if (cbufp)
			m_pContext->PSSetConstantBuffers( 1, 1, &cbufp );

		ID3D11ShaderResourceView *textures[2] = {m_font->GetTexture(),0};
		m_pContext->PSSetShaderResources(0, 1, textures);		// Set the texture
		m_pContext->PSSetSamplers(0, 1, &m_pSampleState);			// Set its texture states..
		m_pContext->IASetInputLayout( stv->VertexLayout() );

		// Initiate default.
		UINT stride[2]	= {stv->VertexStride(),0};
		UINT offset[2]	= {0,0};	// changes rarely
		ID3D11Buffer* bufferPointers[2] = {m_pVertexBuffer,0};

		DXGI_FORMAT val = DXGI_FORMAT_R16_UINT;
		FLOAT bfactor[] = {1.0f, 1.0f, 1.0f, 1.0f };

		m_pContext->IASetIndexBuffer( m_pIndexBuffer, val, 0 );
		m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		//pContext->OMSetBlendState( m_pBlendState, bfactor, 0xfff);	//donne( pBS );
		//pContext->OMSetDepthStencilState( m_pDepthStencilState, 1 );

		//if (isInstancing)
		//{
		//	stride[1]			= stv->InstanceStride();
		//	bufferPointers[1]	= pMaterial->s_pInstanceBuffer;
		//	pContext->IASetVertexBuffers( 0, 2, bufferPointers, stride, offset );
		//	pContext->DrawIndexedInstanced( pMaterial->m_nIndex, pMaterial->s_pInstanceData.size(),0,0,0);
		//	cnt += pMaterial->s_pInstanceData.size();
		//	optcnt++;
		//}
		//else
		{
			m_pContext->IASetVertexBuffers( 0, 1, bufferPointers, stride, offset );
			m_pContext->DrawIndexed(m_nIndex,0,0);
		}

	}
	// Render the text using the font shader.
	//result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),
	//							  pixelColor);


	return width;*/
	return true;
}


}

